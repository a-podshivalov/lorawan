#ifndef VEMAC_HELPER_H
#define VEMAC_HELPER_H

#include "ns3/net-device.h"
#include "ns3/lora-channel.h"
#include "ns3/lora-phy.h"
#include "ns3/lorawan-mac.h"
#include "ns3/lorawan-mac-helper.h"
#include "ns3/vemac-lora.h"
#include "ns3/node-container.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {
namespace lorawan {

class VeMacHelper: public LorawanMacHelper
{
  class VeMacIdGenerator: public Object
  {
  public:
    static TypeId GetTypeId (void);
    VeMacIdGenerator ();
    uint8_t GetNextId();
  private:
    uint8_t m_id;
  };

public:
  /**
   * Create a mac helper without any parameter set. The user must set
   * them all to be able to call Install later.
   */
  VeMacHelper ();

  /**
   * Set an attribute of the underlying MAC object.
   *
   * \param name the name of the attribute to set.
   * \param v the value of the attribute.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * Create the VeMac instance and connect it to a device
   *
   * \param node the node on which we wish to create a wifi MAC.
   * \param device the device within which this MAC will be created.
   * \returns a newly-created VeMac object.
   */
  virtual Ptr<LorawanMac> Create (Ptr<Node> node, Ptr<NetDevice> device) const;

private:

  ObjectFactory m_mac;
  Ptr<VeMacIdGenerator> addrGen;
};

} // namespace lorawan

} // namespace ns3
#endif /* LORA_PHY_HELPER_H */
