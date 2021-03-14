

#include "ns3/vemac-lora-header.h"
#include "ns3/log.h"
#include <ctime>
//#include <bitset>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("VeMacLoraHeader");

VeMacLoraHeader::VeMacLoraHeader ()
{
   for (int i = 0; i < 10; i++)
     {
       m_slots_id[i] = 0;
     }

}

VeMacLoraHeader::~VeMacLoraHeader ()
{
}

TypeId
VeMacLoraHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("VeMacLoraHeader")
    .SetParent<Header> ()
    .AddConstructor<VeMacLoraHeader> ()
  ;
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

  return sizeof(m_slots_id)/sizeof(m_slots_id[0]);
}

void
VeMacLoraHeader::Print (std::ostream &os) const
{
  os << "MyVeMacID=" << unsigned(m_slots_id[10]) << std::endl;
}

void
VeMacLoraHeader::Serialize (Buffer::Iterator start) const
{

  Buffer::Iterator i = start;
//  i.WriteU8 (m_id);

  for (int j = 0; j < 10; j++)
    {
      i.WriteU8 (m_slots_id[j]);
    }
}

uint32_t
VeMacLoraHeader::Deserialize (Buffer::Iterator start)
{
  uint32_t byteRead = 0;

//  //FIrst block of 1 bytes
//  m_id = start.ReadU8();
//  byteRead += 1;

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
