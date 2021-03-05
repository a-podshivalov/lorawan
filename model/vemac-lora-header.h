

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
  /**
   * The message type.
   *
   * The enum value corresponds to the value that will be written in the header
   * by the Serialize method.
   */
  enum MType
  {
    JOIN_REQUEST = 0,
    JOIN_ACCEPT = 1,
    UNCONFIRMED_DATA_UP = 2,
    UNCONFIRMED_DATA_DOWN = 3,
    CONFIRMED_DATA_UP = 4,
    CONFIRMED_DATA_DOWN = 5,
    PROPRIETARY = 7
  };

//  static TypeId GetTypeId (void);

  VeMacLoraHeader ();
  ~VeMacLoraHeader ();

  // Pure virtual methods from Header that need to be implemented by this class
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serialize the header.
   *
   * See Page 15 of LoRaWAN specification for a representation of fields.
   *
   * \param start A pointer to the buffer that will be filled with the
   * serialization.
   */
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

  bool IsUplink (void) const;

  bool IsConfirmed (void) const;

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
