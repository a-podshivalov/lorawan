

#ifndef LORAWAN_MAC_H
#define LORAWAN_MAC_H

#include "ns3/object.h"
#include "ns3/logical-lora-channel-helper.h"
#include "ns3/packet.h"
#include "ns3/lora-phy.h"
#include <array>


using namespace  ns3;
using namespace lorawan;

//class LoraPhy;

/**
 * Class representing the LoRaWAN MAC layer.
 *
 * This class is meant to be extended differently based on whether the layer
 * belongs to an End Device or a Gateway, while holding some functionality that
 * is common to both.
 */
class VeMac : public Object
{
public:
  static TypeId GetTypeId (void);

  VeMac ();
  virtual ~VeMac ();

  typedef std::array<std::array<uint8_t, 6>, 8> ReplyDataRateMatrix;

protected:
//  /**
//   * The PHY instance that sits under this MAC layer.
//   */
  Ptr<LoraPhy> m_phy;
//
//  /**
//   * The device this MAC layer is installed on.
//   */
  Ptr<NetDevice> m_device;


};


#endif /* LORAWAN_MAC_H */
