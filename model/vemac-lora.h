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
  void ApplyNecessaryOptions (VeMacLoraHeader &macHeader);
  int GetRandomNumber();
  void AcquireFreeTimeSlot();

  void TimeSlotOver();
  void DoSend (Ptr<Packet> packet);
  bool CheckCollision (int* slots_copy);

  void SetId (uint8_t id);
  uint8_t GetId (void) const;

  static int GetCurrentTimeSlot(void);

  void CopySlots (VeMacLoraHeader* mHdr, int* slots_copy);


  void SaveOurNxTx (int* slots_copy, int slot_id);
private:
  /*
   * Слоты в кадре
   */
  int m_slots[9];

  /**
   * The VeMac ID of this device.
   */
  uint8_t m_id;

  bool m_receivedOk;
  Ptr<Packet> m_packet;
  //Чтобы смотреть, в каких временных слотах был приём или не был
  bool m_slots_received[10];

  //Чтобы смотреть в конце каждой секунды, есть ли свободный временной слот
  bool m_slot_free;
  //Чтобы запоминать номер временного слота, который мы заняли
  int m_time_slot;

  int slot_listened_for_check;
  int slot_listened_for_initial_assign;

  uint16_t fcnt;
  int lcnt;

  bool m_send;

};
}}

#endif /* VEMAC_LORA_H */
