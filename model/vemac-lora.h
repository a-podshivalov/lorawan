#ifndef VEMAC_LORA_H
#define VEMAC_LORA_H

#include "ns3/lorawan-mac.h"
#include "ns3/lorawan-mac-header.h"
#include "ns3/lora-frame-header.h"
#include "ns3/random-variable-stream.h"
#include "ns3/lora-device-address.h"
#include "ns3/traced-value.h"
#include "ns3/packet.h"
#include "ns3/vemac-lora-header.h"

namespace ns3 {
namespace lorawan {

/**
 * Class representing the MAC layer of a VeMAC-LoRa device.
 */
class VeMacLora : public LorawanMac
{
public:
  static TypeId GetTypeId (void);

  VeMacLora ();
  virtual ~VeMacLora ();

  virtual void Receive (Ptr<Packet const> packet);
  virtual void Send (Ptr<Packet> packet);
  virtual void FailedReception (Ptr<Packet const> packet);
  virtual void TxFinished (Ptr<const Packet> packet);

  void TimeSlotOver();
  void DoSend (Ptr<Packet> packet);

  void SetId (uint8_t id);
  uint8_t GetId (void) const;

  static int GetCurrentTimeSlot(void);
private:
  /*
   * Слоты в кадре
   */
  char m_slots[10];

  /**
   * The VeMac ID of this device.
   */
  uint8_t m_id;

  bool m_receivedOk;
  Ptr<Packet> m_packet;

};
}}

#endif /* VEMAC_LORA_H */
