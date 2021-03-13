
#include "vemac-lora.h"
#include "ns3/vemac.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>
//#include "ns3/class-a-end-device-lorawan-mac.h"

using namespace  ns3;





NS_LOG_COMPONENT_DEFINE ("VeMacLora");



VeMacLora::VeMacLora ()
{
  NS_LOG_FUNCTION (this);

  slots[0] = 'o';
  slots[1] = 'o';
  slots[2] = 'o';
  slots[3] = 'o';
  slots[4] = 'o';
  slots[5] = 'o';
  slots[6] = 'o';
  slots[7] = 'o';
  slots[8] = 'o';
  slots[9] = 'o';

  //Инициализируем свой собственный VeMac ID
  srand( time (0) );
  vemac_id = rand() % 256;



}

VeMacLora::~VeMacLora ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

////////////////////////
//  Sending methods   //
////////////////////////



void
VeMacLora::DoSend (Ptr<Packet> packet) {
  NS_LOG_FUNCTION ("Starting Send");
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
  uint8_t slot_id = (Simulator::Now().GetSeconds() - floor(Simulator::Now().GetSeconds())) * 10;

  //Записываем в найденную ячейку ID того, кого слышали
  mHdr.m_slots_id[slot_id] = mHdr.m_slots_id[10];

  for (int i =0; i<11; i++)
    {
      if (mHdr.m_slots_id[i] == 0)
          {
          mHdr.m_slots_id[10] = vemac_id;
          packetCopy->AddHeader(mHdr);
          Simulator::Schedule(Seconds(1+floor(Simulator::Now().GetSeconds()))+Seconds(0.1)*i, &VeMacLora::DoSend, this, packetCopy);
          break;
          }
    }

}

