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
//  m_slots[9] = 255;

  m_time_slot = -1;
  m_slot_free = false;
  lcnt = 0;

  for (int i = 0; i < 10; i ++)
    {
      m_slots_received[i] = false;
    }

  fcnt = 0;

  m_send = false;

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
  if ( slot_listened_for_initial_assign >= 10 && m_time_slot == -1 )
      {
        for (int i =0; i < 10; i++)
          {
            if (m_slots_received[i] == false)
              {
                m_slot_free = true;
                break;
              }
          }
      }


    while (m_time_slot == -1 && m_slot_free)
      {
        int our_time_slot = GetRandomNumber();
        if (m_slots_received[our_time_slot] == false)
          {
            m_time_slot = our_time_slot;
            NS_LOG_INFO((int)m_id << " ASSIGN TIME SLOT № " << m_time_slot);
            slot_listened_for_check = 0;
            slot_listened_for_initial_assign = 0;
            m_send = false;
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
//  int past_time_slot = (current_time_slot == 0 && Simulator::Now().GetSeconds() >= 2 ? 9 : current_time_slot-1);

  m_slots_received[current_time_slot] = false;

  slot_listened_for_initial_assign++;
  slot_listened_for_check++;

  AcquireFreeTimeSlot();

  if ( current_time_slot == 0)
    {
      fcnt++;
    }

  // TODO: if it is our selected time slot - DoSend
  // TODO: write an actual condition
  if(m_packet && current_time_slot == m_time_slot){
      DoSend(m_packet);
  }



  // else - check whether a packet was received
//  if(!m_slots_received[past_time_slot] && past_time_slot != m_time_slot)
//    {
//      m_slots_received[past_time_slot] = false;
//     }

  if (slot_listened_for_check > 50 && m_time_slot != -1)
          {
              NS_LOG_INFO("Try to receive new time slot");
              bool empty = true;
              for (int i = 0; i < 9; ++i)
              {
                  if (m_slots[i] != 255)
                  {
                      empty = false;
                      break;
                  }
              }

              if (empty)
              {
                  m_time_slot = -1;
                  AcquireFreeTimeSlot();
              }
          }


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

  m_send = true;
}

bool
VeMacLora::CheckCollision (int* slots_copy) {
  bool his_id_in_nx = false;

      for (int i = 0; i <= 8; ++i)
      {
          if (slots_copy[9] == m_slots[i] /*&& m_slots_received[i]*/)
          {
              his_id_in_nx = true;
          }
      }


      if (his_id_in_nx)
      {
          for (int i = 0; i <= 8; ++i)
              {
                  if (slots_copy[i] == m_id) return true;
              }
      }
      else
          return true;

      return false;
}

void
VeMacLora::CopySlots (VeMacLoraHeader* mHdr, int* slots_copy)
{

  for (int i = 0; i <= 4; i++)
    {
      slots_copy[i] = mHdr->m_slots_id[i+1];
    }

  for (int i = 8; i < 14; i++)
    {
      slots_copy[i-4] = mHdr->m_slots_id[i];
    }

}


void
VeMacLora::SaveOurNxTx (int* slots_copy, int slot_id)
{
  m_slots_received[slot_id] = true;

  bool node_our_nx_tx = false;

  for (int i = 0; i < 10; i++)
    {
      node_our_nx_tx = false;
      for (int j = 0; j < 9; j++)
        {
          if (slots_copy[i] == m_id)
            {
              node_our_nx_tx = true;
              break;
            }

          if (slots_copy [i] == m_slots[j])
              {
               node_our_nx_tx = true;
               break;
              }
        }
      if (!node_our_nx_tx)
      {
        m_slots[lcnt] = slots_copy [i];

        lcnt++;
      }
      lcnt = ( lcnt == 9 ? 0 : lcnt);

    }
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

  int slots_copy[10];

  CopySlots(&mHdr, slots_copy);

  NS_LOG_INFO("Copy: " << slots_copy[0]<< " " <<  slots_copy[1]<< " " << slots_copy[2] << " " << slots_copy[3] << " " << slots_copy[4] << " " << slots_copy[5] << " " <<  slots_copy[6] << " " << slots_copy[7] << " " << slots_copy[8] << " " << slots_copy[9]);

  //Обнаружена коллизия, освобождаем занятую нами временную ячейку
  if (!CheckCollision(slots_copy) && m_send /*|| slot_id == m_time_slot*/)
      {
        NS_LOG_INFO("COLLISION DETECTION");


//        if (slot_id != m_time_slot)
//          {
//            m_slots_received[slot_id] = true;
//          }

        m_time_slot = -1;
        m_send = false;
      }
      else
      {
          NS_LOG_INFO("Save received Vemac ID: " << (int)mHdr.m_slots_id[13]);

          SaveOurNxTx(slots_copy, slot_id);

          lcnt = ( lcnt == 9 ? 0 : lcnt);


         slot_listened_for_check = 0;
      }
      NS_LOG_INFO("I Heard: " << m_slots[0]<< " " <<  m_slots[1]<< " " << m_slots[2] << " " << m_slots[3] << " " << m_slots[4] << " " << m_slots[5] << " " <<  m_slots[6] << " " << m_slots[7] << " " << m_slots[8]);

  NS_LOG_DEBUG(std::endl);
}

void
VeMacLora::ApplyNecessaryOptions (VeMacLoraHeader& macHeader)
{
  NS_LOG_FUNCTION_NOARGS ();

  macHeader.m_slots_id[0] = 0xE0;

  for (int i = 0; i < 4; i++)
    {
      macHeader.m_slots_id[i+1] = m_slots[i];
    }

  macHeader.m_slots_id[5] = 0x06;

  macHeader.m_slots_id[6] = fcnt & 0xff;
  macHeader.m_slots_id[7] = (fcnt >> 8) & 0xff;

  for (int i = 4; i < 9; i++)
      {
        macHeader.m_slots_id[i+4] = m_slots[i];
      }

  //Пишем в заголовок наш VeMac ID
  macHeader.m_slots_id[13] = m_id;
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
