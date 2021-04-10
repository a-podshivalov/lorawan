#include "ns3/vemac-lora-header.h"
#include "ns3/log.h"
#include <ctime>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE("VeMacLoraHeader");

VeMacLoraHeader::VeMacLoraHeader ()
{
  for (int i = 0; i < 10; i++)
    {
      m_slots_id[i] = 255;
    }

  m_fcnt = 0;
}

VeMacLoraHeader::~VeMacLoraHeader ()
{
}

TypeId
VeMacLoraHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("VeMacLoraHeader").SetParent<Header> ().AddConstructor<
      VeMacLoraHeader> ();
  return tid;
}

TypeId
VeMacLoraHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
VeMacLoraHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 14;
}

void
VeMacLoraHeader::Print (std::ostream &os) const
{
  os << "VeMac Header from ID=" << unsigned (m_slots_id[9]) << std::endl;
  os << "H: ";
  for (int i = 0; i < 10; i++)
    {
      os << unsigned (m_slots_id[i]) << " ";
    }
  os << std::endl;
}

void
VeMacLoraHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (0xE0); /* mHdr in LoRaWAN */

  for (int j = 0; j < 4; j++)
    {
      i.WriteU8 (m_slots_id[j]);
    }

  i.WriteU8 (0x06); /* fOptsLen in LoRaWAN */
  i.WriteU16 (m_fcnt); /* fCnt both in VeMAC-LoRa and LoRaWAN */

  for (int j = 4; j < 10; j++)
    {
      i.WriteU8 (m_slots_id[j]);
    }
}

uint32_t
VeMacLoraHeader::Deserialize (Buffer::Iterator start)
{
  uint32_t byteRead = 0;

  uint8_t mHdr = start.ReadU8 ();
  byteRead++;

  for (int j = 0; j < 4; j++)
    {
      m_slots_id[j] = start.ReadU8 ();
      byteRead++;
    }

  uint8_t fCtrl = start.ReadU8 ();
  byteRead++;
  m_fcnt = start.ReadU16 ();
  byteRead += 2;

  for (int j = 4; j < 10; j++)
    {
      m_slots_id[j] = start.ReadU8 ();
      byteRead++;
    }

  // Get rid of unused warnings
  // do not need to assert if the header is valid
  // now we do not test for LoRaWAN interoperability
  (void) mHdr;
  (void) fCtrl;
  return byteRead;
}

}
}
