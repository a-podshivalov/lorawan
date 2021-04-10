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
  static TypeId
  GetTypeId (void);

  VeMacLora ();

  virtual
  ~VeMacLora ();

  virtual void
  Receive (Ptr<Packet const> packet);

  virtual void
  Send (Ptr<Packet> packet);

  virtual void
  FailedReception (Ptr<Packet const> packet);

  virtual void
  TxFinished (Ptr<const Packet> packet);

  void
  ApplyNecessaryOptions (VeMacLoraHeader &macHeader);

  bool
  AcquireFreeTimeSlot ();

  void
  TimeSlotOver ();

  void
  DoSend (Ptr<Packet> packet);

  bool
  IsCollision (VeMacLoraHeader &macHeader);

  void
  SetId (uint8_t id);

  uint8_t
  GetId (void) const;
private:
  Ptr<Packet> m_packet;

  uint16_t m_fcnt;
  int m_scnt;
  //Чтобы запоминать номер временного слота, который мы заняли
  int m_time_slot;

  /*
   * Слоты в кадре
   */
  int m_slots[10];
  //Чтобы смотреть, в каких временных слотах был приём или не был
  bool m_slots_received[10];
  bool m_received, m_transmitted;

  int m_slots_listened;

  /**
   * The VeMac ID of this device.
   */
  uint8_t m_id;

  Ptr<UniformRandomVariable> rng;
};
}
}

#endif /* VEMAC_LORA_H */
