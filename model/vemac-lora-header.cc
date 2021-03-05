

#include "ns3/vemac-lora-header.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("VeMacLoraHeader");

VeMacLoraHeader::VeMacLoraHeader () : m_major (0)
{
}

VeMacLoraHeader::~VeMacLoraHeader ()
{
}

//TypeId
//VeMacLoraHeader::GetTypeId (void)
//{
//  static TypeId tid = TypeId ("VeMacLoraHeader")
//    .SetParent<Header> ()
//    .AddConstructor<VeMacLoraHeader> ()
//  ;
//  return tid;
//}

TypeId
VeMacLoraHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
VeMacLoraHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 1;       // This header only consists in 8 bits
}

void
VeMacLoraHeader::Serialize (Buffer::Iterator start) const
{

}

uint32_t
VeMacLoraHeader::Deserialize (Buffer::Iterator start)
{

}


}
}
