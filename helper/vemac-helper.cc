/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include "ns3/vemac-helper.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/lora-net-device.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("VeMacHelper");

TypeId
VeMacHelper::VeMacIdGenerator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VeMacHelper::VeMacIdGenerator")
    .SetParent<Object> ()
    .SetGroupName ("lorawan")
    .AddConstructor<VeMacHelper::VeMacIdGenerator> ();
  return tid;
}

VeMacHelper::VeMacIdGenerator::VeMacIdGenerator(void): m_id(0) {}

uint8_t VeMacHelper::VeMacIdGenerator::GetNextId(void) { return m_id++; }

VeMacHelper::VeMacHelper ()
{
  addrGen = CreateObject<VeMacHelper::VeMacIdGenerator>();
  m_mac.SetTypeId ("ns3::VeMacLora");
}

void
VeMacHelper::Set (std::string name, const AttributeValue &v)
{
  m_mac.Set (name, v);
}

Ptr<LorawanMac>
VeMacHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  Ptr<LorawanMac> mac = m_mac.Create<LorawanMac> ();
  mac->SetDevice (device);

  mac->GetObject<VeMacLora> ()->SetId (addrGen->GetNextId());

  return mac;
}

} // namespace lorawan
} // namespace ns3
