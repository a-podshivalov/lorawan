#include "vemac-lora.h"
#include "ns3/vemac.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>

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

  Simulator::Schedule(Seconds(1), &VeMacLora::TimeSlotOver, this);
}

VeMacLora::~VeMacLora ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

int VeMacLora::GetCurrentTimeSlot(void)
{
  return (Simulator::Now().GetMilliSeconds() % 1000) / 100;
}

void VeMacLora::TimeSlotOver()
{
  // Schedule a next time slot event
  Simulator::Schedule(MilliSeconds(100), &VeMacLora::TimeSlotOver, this);

  // TODO: if it is our selected time slot - DoSend
  // TODO: write an actual condition
  if(m_packet && GetCurrentTimeSlot() == m_id){
      DoSend(m_packet);
  }

  // else - check whether a packet was received
  if(!m_receivedOk){
      // do we need to mark this timeslot?
  }

  m_receivedOk = false;

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
  VeMacLoraHeader frameHdr;
  // TODO: add VeMac header properly
  // fill this header with actual values
  frameHdr.m_slots_id[9] = m_id+10;
  packet->AddHeader (frameHdr);

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

void
VeMacLora::Receive (Ptr<Packet const> packet) {
  NS_LOG_FUNCTION (this << packet);

  // Work on a copy of the packet
  Ptr<Packet> packetCopy = packet->Copy ();

  // Получаем заголовок пакета
  VeMacLoraHeader mHdr;
  packetCopy->RemoveHeader(mHdr);
  NS_LOG_DEBUG("Mac Header: " << mHdr);

  //Получаем номер ячейки, в которое запишем Vemac ID того, кого слышали
  uint8_t slot_id = GetCurrentTimeSlot();

  //Записываем в найденную ячейку ID того, кого слышали
  mHdr.m_slots_id[slot_id] = mHdr.m_slots_id[10];

  for (int i =0; i<11; i++)
    {
      if (mHdr.m_slots_id[i] == 0)
        {
          mHdr.m_slots_id[10] = m_id;
          packetCopy->AddHeader(mHdr);

          break;
        }
    }

  m_receivedOk = true;
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
