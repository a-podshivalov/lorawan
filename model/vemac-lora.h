

#ifndef END_DEVICE_LORAWAN_MAC_H
#define END_DEVICE_LORAWAN_MAC_H

#include "ns3/vemac.h"
#include "ns3/lorawan-mac-header.h"
#include "ns3/lora-frame-header.h"
#include "ns3/random-variable-stream.h"
#include "ns3/lora-device-address.h"
#include "ns3/traced-value.h"
#include "ns3/packet.h"
#include "ns3/vemac-lora-header.h"

using namespace  ns3;


/**
 * Class representing the MAC layer of a LoRaWAN device.
 */
class VeMacLora : public Object
{
public:
  static TypeId GetTypeId (void);

  VeMacLora ();
  ~VeMacLora ();

  void ScheduleTdma (const uint32_t slotNum);
  virtual void Receive (Ptr<Packet const> packet);
  virtual void DoSend (Ptr<Packet> packet);

  /*
   * Слоты в кадре
   */



  /**
   * The address of this device.
   */
//  LoraDeviceAddress m_id; //Здеcь был m_address
  uint8_t vemac_id;

private:
  char slots[10];

};




#endif /* END_DEVICE_LORAWAN_MAC_H */
