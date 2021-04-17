/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 University of Padova
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

#ifndef VEMAC_TRACKER_HELPER_H
#define VEMAC_TRACKER_HELPER_H

#include "ns3/packet.h"
#include "ns3/nstime.h"

#include <map>
#include <string>

namespace ns3 {
namespace lorawan {


struct VeMacPacketStatus
{
  Ptr<Packet const> packet;
  uint32_t senderId;
  Time sendTime;
  Time receivedTime;
  std::map<int, Time> receptionTimes;
};

typedef std::map<Ptr<Packet const>, VeMacPacketStatus> VeMacPacketData;

class VemacTrackerHelper
{
public:
  VemacTrackerHelper ();
  ~VemacTrackerHelper ();
  VeMacPacketData m_macPacketTracker;
};
}
}
#endif
