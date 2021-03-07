

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

  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the header.
   *
   * \param start A pointer to the buffer we need to deserialize.
   * \return The number of consumed bytes.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Print the header in a human readable format.
   *
   * \param os The std::ostream on which to print the header.
   */

private:

  /**
     * The VeMac ID.
     */
    uint8_t m_id;
  /**
   * VeMac ID  других устройств
   */
  uint8_t m_slots_id[10];
};
}

}
#endif
