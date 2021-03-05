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

#include "vemac.h"
#include "ns3/lorawan-mac.h"
#include "ns3/log.h"
#include "ns3/vemac-lora-header.h"


NS_LOG_COMPONENT_DEFINE ("VeMac");
using namespace ns3;

VeMac::VeMac ()
{
  NS_LOG_FUNCTION (this);
}

VeMac::~VeMac ()
{
  NS_LOG_FUNCTION (this);
}