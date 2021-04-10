#ifndef VEMAC_LORA_HEADER_H
#define VEMAC_LORA_HEADER_H

#include "ns3/header.h"

namespace ns3 {
namespace lorawan {

/**
 * This class represents the Mac header of a VeMac packet.
 */
class VeMacLoraHeader : public Header
{
public:
  static TypeId
  GetTypeId (void);

  VeMacLoraHeader ();
  ~VeMacLoraHeader ();

  virtual uint32_t
  GetSerializedSize (void) const;
  virtual TypeId
  GetInstanceTypeId (void) const;

  virtual void
  Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the header.
   *
   * \param start A pointer to the buffer we need to deserialize.
   * \return The number of consumed bytes.
   */
  virtual uint32_t
  Deserialize (Buffer::Iterator start);

  /**
   * Print the header in a human readable format.
   *
   * \param os The std::ostream on which to print the header.
   *
   */
  virtual void
  Print (std::ostream &os) const;

  /**
   * VeMac ID  других устройств,  [10] - это будет VemacID самого устройства
   */
  uint8_t m_slots_id[10];
  uint16_t m_fcnt;

private:
};
}

}
#endif
