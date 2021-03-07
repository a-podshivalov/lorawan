

#include "ns3/vemac-lora-header.h"
#include "ns3/log.h"
//#include <bitset>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("VeMacLoraHeader");






void
VeMacLoraHeader::Serialize (Buffer::Iterator start) const
{

  Buffer::Iterator i = start;
  i.WriteU8 (m_id);

  for (int j = 0; j < 10; j++)
    {
      i.WriteU8 (m_slots_id[j]);
    }

}

uint32_t
VeMacLoraHeader::Deserialize (Buffer::Iterator start)
{
  uint32_t byteRead = 0;

  //FIrst block of 1 bytes
  m_id = start.ReadU8();
  byteRead += 1;

  //block of 10 bytes
  for (int j = 0; j < 10; j++)
    {
      m_slots_id [j] = start.ReadU8();
      byteRead++;
    }

  return byteRead;
}


}
}
