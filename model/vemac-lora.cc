#include "vemac-lora.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE("VeMacLora");

NS_OBJECT_ENSURE_REGISTERED(VeMacLora);

TypeId
VeMacLora::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VeMacLora").SetParent<LorawanMac> ().SetGroupName (
      "lorawan")
  /*.AddAttribute ("Id", "VeMac ID of this device",
   0,
   MakeUintegerAccessor(&VeMacLora::GetId, &VeMacLora::SetId),
   MakeUintegerChecker<uint8_t>(0, 255))*/
  .AddConstructor<VeMacLora> ();
  return tid;
}

VeMacLora::VeMacLora ()
{
  NS_LOG_FUNCTION(this);

  m_time_slot = -1;

  for (int i = 0; i < 10; i++)
    {
      m_slots[i] = 255;
      m_slots_received[i] = false;
    }

  m_fcnt = 0;
  m_scnt = 0;

  m_received = false;
  m_transmitted = false;

  m_slots_listened = 0;

  rng = CreateObject<UniformRandomVariable> (); // Генератор случайных чисел.

  Simulator::Schedule (Seconds (1.1), &VeMacLora::TimeSlotOver, this);
}

VeMacLora::~VeMacLora ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool
VeMacLora::AcquireFreeTimeSlot ()
{
  // like acquire_free_time_slot
  int free_slots = 0;
  for (int i = 0; i < 10; i++)
    {
      if (m_slots[i] == 255)
        {
          free_slots++;
        }
    }

  if (free_slots > 0)
    {
      free_slots = rng->GetInteger (0, free_slots);

      for (int i = 0; i < 10; i++)
        {
          if (m_slots[i] == 255)
            {
              if (free_slots-- == 0)
                {
                  m_time_slot = i;
                  NS_LOG_DEBUG((int)m_id << " acquired time slot № " << m_time_slot);
                  m_slots_listened = 0;
                  m_transmitted = false;

                  return true;
                }
            }
        }
    }

  return false;
}

void
VeMacLora::TimeSlotOver ()
{
  // Schedule a next time slot event
  Simulator::Schedule (MilliSeconds (100), &VeMacLora::TimeSlotOver, this);

  m_scnt++;
  if (m_scnt >= 10)
    {
      m_fcnt++;
      m_scnt = 0;
    }
  // like slot_thread body:
  int prev_time_slot = (m_scnt == 0) ? 9 : m_scnt - 1;
  if (!m_received)
    {
      m_slots_received[prev_time_slot] = false;
    }
  m_received = false;

  m_slots_listened++;
  if (m_slots_listened >= 2 * 10)
    {
      bool res = AcquireFreeTimeSlot ();

      if (!res)
        {
          NS_LOG_DEBUG("Failed to acquire time slot");
        }
    }

  // TODO: get rid of constants, set them as parameters of VeMac object
  m_phy->GetObject<EndDeviceLoraPhy> ()->SetFrequency (869.000);
  m_phy->GetObject<EndDeviceLoraPhy> ()->SetSpreadingFactor (7);

  if (m_packet && m_scnt == m_time_slot)
    {
      DoSend (m_packet);
    }
  else
    {
      // Switch the PHY to standby so that it will listen here for downlink
      m_phy->GetObject<EndDeviceLoraPhy> ()->SwitchToStandby ();
    }
}

void
VeMacLora::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION(this << packet);

  m_packet = packet;
}

void
VeMacLora::DoSend (Ptr<Packet> packet)
{
  VeMacLoraHeader macHdr;
  ApplyNecessaryOptions (macHdr);
  packet->AddHeader (macHdr);
  NS_LOG_DEBUG("Sending, my VeMac ID is " << (int)m_id);

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

  NS_LOG_INFO(
      "PacketToSend: " << packet << " Duration: " << duration.GetMilliSeconds() << " ms");
  m_phy->Send (packet, params, 869.000, 14);

  m_transmitted = true;
}

bool
VeMacLora::IsCollision (VeMacLoraHeader &macHeader)
{
  // like is_collision
  bool his_id_in_nx = false;

  for (int i = 0; i < 10; ++i)
    {
      if (macHeader.m_slots_id[9] == m_slots[i] && m_slots_received[i])
        {
          his_id_in_nx = true;
        }
    }

  if (his_id_in_nx)
    {
      NS_LOG_LOGIC("A packet from a neighbor received!");
      for (int i = 0; i < 9; ++i)
        {
          if (macHeader.m_slots_id[i] == m_id)
            {
              return false;
            }
        }
      return true;
    }

  return false;
}

void
VeMacLora::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION(this << packet);

  // Work on a copy of the packet
  Ptr<Packet> packetCopy = packet->Copy ();

  // Получаем заголовок пакета
  VeMacLoraHeader mHdr;
  packetCopy->RemoveHeader (mHdr);
  NS_LOG_INFO((int)m_id <<" received packet from " << mHdr);
  m_received = true;

  //Обнаружена коллизия, освобождаем занятую нами временную ячейку
  if (m_transmitted && IsCollision (mHdr))
    {
      NS_LOG_INFO("COLLISION DETECTED");
      m_slots[m_time_slot] = 255;
      m_time_slot = -1;
      m_transmitted = false;
    }
  else
    {
      if (m_time_slot != -1)
        {
          m_slots_listened = 0;
        }
    }

  m_slots[m_scnt] = mHdr.m_slots_id[9];
  m_slots_received[m_scnt] = true;

  for (int i = 0; i < 9; i++)
    {
      int lcnt = (m_scnt + 1 + i) % 10;
      if (!m_slots_received[i] && lcnt != m_time_slot) /* Do not overwrite neighbors and self */
        {
          m_slots[lcnt] = mHdr.m_slots_id[i];
        }
    }

  NS_LOG_DEBUG(
      "I am " << +m_id <<" I Heard: " << m_slots[0]<< " " << m_slots[1]<< " " << m_slots[2] << " " << m_slots[3] << " " << m_slots[4] << " " << m_slots[5] << " " << m_slots[6] << " " << m_slots[7] << " " << m_slots[8] << " " << m_slots[9]);
}

void
VeMacLora::ApplyNecessaryOptions (VeMacLoraHeader &macHeader)
{
  NS_LOG_FUNCTION_NOARGS ();

  macHeader.m_fcnt = m_fcnt;
  for (int i = 0; i < 9; i++)
    {
      int lcnt = (m_scnt + i + 1) % 10;
      if (m_slots_received[lcnt])
        {
          macHeader.m_slots_id[i] = m_slots[lcnt];
        }
      else
        {
          macHeader.m_slots_id[i] = 255;
        }
    }

  //Пишем в заголовок наш VeMac ID
  macHeader.m_slots_id[9] = m_id;
}

void
VeMacLora::FailedReception (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION_NOARGS ();

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

}
}
