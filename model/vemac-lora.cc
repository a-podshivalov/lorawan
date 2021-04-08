#include "vemac-lora.h"
#include "ns3/vemac.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>
#include <random>


namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("VeMacLora");

NS_OBJECT_ENSURE_REGISTERED (VeMacLora);

TypeId
VeMacLora::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VeMacLora")
    .SetParent<LorawanMac> ()
    .SetGroupName ("lorawan")
    /*.AddAttribute ("Id", "VeMac ID of this device",
                       0,
                       MakeUintegerAccessor(&VeMacLora::GetId, &VeMacLora::SetId),
                       MakeUintegerChecker<uint8_t>(0, 255))*/
    .AddConstructor<VeMacLora> ();
  return tid;
}


VeMacLora::VeMacLora () : m_receivedOk(false)
{
  NS_LOG_FUNCTION (this);

  m_slots[0] = 255;
  m_slots[1] = 255;
  m_slots[2] = 255;
  m_slots[3] = 255;
  m_slots[4] = 255;
  m_slots[5] = 255;
  m_slots[6] = 255;
  m_slots[7] = 255;
  m_slots[8] = 255;
  m_slots[9] = 255;

  m_time_slot = -1;
  m_slot_free = false;

  for (int i = 0; i < 10; i ++)
    {
      m_slots_received[i] = false;
    }

  slot_listened_for_initial_assign = 0;
  slot_listened_for_check=0;

  Simulator::Schedule(Seconds(1.1), &VeMacLora::TimeSlotOver, this);
}

VeMacLora::~VeMacLora ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

int VeMacLora::GetCurrentTimeSlot(void)
{
  return (Simulator::Now().GetMilliSeconds() % 1000) / 100;
}

int VeMacLora::GetRandomNumber()
{
  std::random_device random_device; // Источник энтропии.
  std::mt19937 generator(random_device()); // Генератор случайных чисел.
  // (Здесь берется одно инициализирующее значение, можно брать больше)

  std::uniform_int_distribution<> distribution(0, 9); // Равномерное распределение [0, 9]

  return distribution(generator);
}

void VeMacLora::AcquireFreeTimeSlot()
{
  if ( slot_listened_for_initial_assign == 10 && m_time_slot == -1 )
      {
        for (int i =0; i < 10; i++)
          {
            if (m_slots[i] == 255)
              {
                m_slot_free = true;
                break;
              }
          }
      }


    while (m_time_slot == -1 && m_slot_free)
      {
        int out_time_slot = GetRandomNumber();
        if (m_slots[out_time_slot] == 255)
          {
            m_slots[out_time_slot] = m_id;
            m_time_slot = out_time_slot;
            NS_LOG_INFO((int)m_id << " ASSIGN TIME SLOT № " << m_time_slot);
            slot_listened_for_check = 0;
            break;
          }
      }
}

void VeMacLora::TimeSlotOver()
{
  // Schedule a next time slot event
  Simulator::Schedule(MilliSeconds(100), &VeMacLora::TimeSlotOver, this);

//  NS_LOG_DEBUG("Scheduling time slots");
  int current_time_slot = GetCurrentTimeSlot();
  int past_time_slot = (current_time_slot == 0 && Simulator::Now().GetSeconds() >= 2 ? 9 : current_time_slot-1);

  slot_listened_for_initial_assign++;
  slot_listened_for_check++;

  AcquireFreeTimeSlot();

  // TODO: if it is our selected time slot - DoSend
  // TODO: write an actual condition
  if(m_packet && current_time_slot == m_time_slot){
      DoSend(m_packet);
  }

  // else - check whether a packet was received
  if(!m_slots_received[past_time_slot] && past_time_slot != m_time_slot)
    {
          m_slots[past_time_slot] = 255;
      }

  if (slot_listened_for_check > 50 && m_time_slot != -1)
          {
              NS_LOG_INFO("Try to receive new time slot");
              bool empty = true;
              for (int i = 0; i < 10; ++i)
              {
                  if (m_slots[i] != 255 && m_time_slot != i)
                  {
                      empty = false;
                      break;
                  }
              }

              if (empty)
              {
                  m_slots[m_time_slot] = 255;
                  m_time_slot = -1;
                  slot_listened_for_initial_assign = 10;
                  AcquireFreeTimeSlot();
              }
          }

  m_slots_received[current_time_slot] = false;
  m_slot_free = false;

  // Switch the PHY to the channel so that it will listen here for downlink
  // TODO: get rid of constants, set them as parameters of VeMac object
  m_phy->GetObject<EndDeviceLoraPhy> ()->SetFrequency (869.000);
  m_phy->GetObject<EndDeviceLoraPhy> ()->SetSpreadingFactor (7);
  m_phy->GetObject<EndDeviceLoraPhy> ()->SwitchToStandby ();
}

void
VeMacLora::Send (Ptr<Packet> packet) {
  NS_LOG_FUNCTION (this << packet);

  m_packet = packet;
}

void
VeMacLora::DoSend (Ptr<Packet> packet) {
  VeMacLoraHeader macHdr;
  // TODO: add VeMac header properly
  // fill this header with actual values


//  frameHdr.m_slots_id[9] = m_id+10;
  ApplyNecessaryOptions(macHdr);
  packet->AddHeader (macHdr);
  NS_LOG_DEBUG("My VeMac ID is " << (int)m_id);

  NS_LOG_INFO("Added MAC header of size " << macHdr.GetSerializedSize() << " bytes. ");

  // TODO: get rid of constants
  // Craft LoraTxParameters object
  LoraTxParameters params;
  params.sf = 7;
  params.headerDisabled = 1;
  params.codingRate = 1;
  params.bandwidthHz = 125000;
  params.nPreamble = 10;
  params.crcEnabled = 1;
  params.lowDataRateOptimizationEnabled = 0;



  // Compute packet duration
  Time duration = m_phy->GetOnAirTime (packet, params);

  NS_LOG_DEBUG ("PacketToSend: " << packet << " Duration: " << duration.GetMilliSeconds() << " ms");
  m_phy->Send (packet, params, 869.000, 14);
}

bool
VeMacLora::CheckCollision (VeMacLoraHeader* mHdr) {
  bool his_id_in_nx = false;

      for (int i = 0; i < 10; ++i)
      {
          if (mHdr->m_slots_id[9] == m_slots[i] && m_slots_received[i])
          {
              his_id_in_nx = true;
          }
      }


      if (his_id_in_nx)
      {
          for (int i = 0; i < 9; ++i)
              {
                  if (mHdr->m_slots_id[i] == m_id) return true;
              }
      }
      else
          return true;

      return false;
}


void
VeMacLora::Receive (Ptr<Packet const> packet) {
  NS_LOG_DEBUG(std::endl);
  NS_LOG_FUNCTION (this << packet);

  // Work on a copy of the packet
  Ptr<Packet> packetCopy = packet->Copy ();


  // Получаем заголовок пакета
  VeMacLoraHeader mHdr;
  packetCopy->RemoveHeader(mHdr);
  NS_LOG_DEBUG((int)m_id <<" received packet from "  << mHdr);

  //Получаем номер ячейки, в которое запишем Vemac ID того, кого слышали
  uint8_t slot_id = GetCurrentTimeSlot();

  //Обнаружена коллизия, освобождаем занятую нами временную ячейку
  if (!CheckCollision(&mHdr) || slot_id == m_time_slot)
      {
        NS_LOG_INFO("Collision detection");
          m_slots[m_time_slot] = 255;
          m_time_slot = -1;
      }
      else
      {
          NS_LOG_INFO("Save received Vemac ID: " << (int)mHdr.m_slots_id[9]);
          m_slots_received[slot_id] = true;
          m_slots[slot_id] = mHdr.m_slots_id[9];

          for(int i = 0; i < 9; i++){
              int lcnt = (slot_id + 1 + i) % 10; /* local counter */
              if( !m_slots_received[lcnt] ){
                  m_slots[lcnt] = mHdr.m_slots_id[i];
              }
          }

         slot_listened_for_check = 0;
      }
      NS_LOG_INFO("I Heard: " << m_slots[0]<< " " <<  m_slots[1]<< " " << m_slots[2] << " " << m_slots[3] << " " << m_slots[4] << " " << m_slots[5] << " " <<  m_slots[6] << " " << m_slots[7] << " " << m_slots[8] << " " << m_slots[8]);

  NS_LOG_DEBUG(std::endl);
}

void
VeMacLora::ApplyNecessaryOptions (VeMacLoraHeader& macHeader)
{
  NS_LOG_FUNCTION_NOARGS ();
  //Пишем в заголовок информацию о временных слотах
  for (int i = 1; i < 10; i++)
    {
      int lcnt = (GetCurrentTimeSlot() + i + 1) % 10;
      if (m_slots_received[lcnt]) {
          macHeader.m_slots_id[i-1] = m_slots[lcnt];
      } else {
          macHeader.m_slots_id[i-1] = 255;
      }

    }
  //Пишем в заголовок наш VeMac ID
  macHeader.m_slots_id[9] = m_id;
}

void
VeMacLora::FailedReception (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_receivedOk = false;

  // Set Phy in Standby mode
  m_phy->GetObject<EndDeviceLoraPhy> ()->SwitchToStandby ();
}

void
VeMacLora::TxFinished (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set Phy in Standby mode
  m_phy->GetObject<EndDeviceLoraPhy> ()->SwitchToStandby ();
}

void
VeMacLora::SetId (uint8_t id)
{
  m_id = id;
}

uint8_t
VeMacLora::GetId (void) const
{
  return m_id;
}

}}
