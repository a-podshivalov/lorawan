/*
 * This script simulates a complex scenario with multiple gateways and end
 * devices. The metric of interest for this script is the throughput of the
 * network.
 */

#include "ns3/end-device-lora-phy.h"
#include "ns3/vemac-lora.h"
#include "ns3/vemac-helper.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/command-line.h"
#include "ns3/network-server-helper.h"
#include "ns3/correlated-shadowing-propagation-loss-model.h"
#include "ns3/building-penetration-loss.h"
#include "ns3/building-allocator.h"
#include "ns3/buildings-helper.h"
#include "ns3/forwarder-helper.h"
#include "ns3/waypoint-mobility-model.h"
#include <algorithm>
#include "ns3/one-shot-sender-helper.h"
#include <ctime>
#include "ns3/network-module.h"
#include "ns3/stats-module.h"
#include "ns3/periodic-sender-helper.h"
#include <cmath>
#include "ns3/random-variable-stream.h"

#define PI 3.14159265
#define TIME 5
#define RADIUS 5000


using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE("VeMacExample");

int
main (int argc, char *argv[])
{
  RngSeedManager::SetSeed (time (NULL));
  Ptr<UniformRandomVariable> rng;
  rng = CreateObject<UniformRandomVariable> (); // Генератор случайных чисел.

  CommandLine cmd;
  cmd.Parse (argc, argv);

  // Set up logging
  //LogComponentEnable ("VeMacExample", LOG_LEVEL_ALL);
  LogComponentEnable ("VeMacLora", LOG_LEVEL_DEBUG);
  //LogComponentEnable ("LoraChannel", LOG_LEVEL_INFO);
  //LogComponentEnable ("LoraPhy", LOG_LEVEL_ALL);
  //LogComponentEnable ("EndDeviceLoraPhy", LOG_LEVEL_ALL);
  //LogComponentEnable ("LoraInterferenceHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("LogicalLoraChannel", LOG_LEVEL_ALL);

  //LogComponentEnable ("LoraPhyHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("LorawanMacHelper", LOG_LEVEL_ALL);
  //LogComponentEnable ("LorawanMacHeader", LOG_LEVEL_ALL);
  //LogComponentEnable ("LoraFrameHeader", LOG_LEVEL_ALL);
  //LogComponentEnable ("PeriodicSender", LOG_LEVEL_ALL);
//   LogComponentEnable ("LoraPacketTracker", LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnableAll (LOG_PREFIX_TIME);
  // LogComponentEnable ("LoraHelper", LOG_LEVEL_ALL);

  /************************
   *  Create the channel  *
   ************************/

  // Create the lora channel object
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  /************************
   *  Create the helpers  *
   ************************/
  // Create the LoraPhyHelper
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the LorawanMacHelper
  VeMacHelper macHelper = VeMacHelper ();

  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();
  helper.EnablePacketTracking ();

  /************************
   *  Create End Devices  *
   ************************/
  NodeContainer nodes;
  nodes.Create (60);

  // Create the LoraNetDevices of the end devices
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (
      nwkId, nwkAddr);

  // // Create the LoraNetDevices of the end devices
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  helper.Install (phyHelper, macHelper, nodes);

  //   // Connect trace sources
  // for (NodeContainer::Iterator j = nodes.Begin (); j != nodes.End (); ++j)
  //   {
  //     Ptr<Node> node = *j;
  //     Ptr<LoraNetDevice> loraNetDevice = node->GetDevice (0)->GetObject<LoraNetDevice> ();
  //     Ptr<LoraPhy> phy = loraNetDevice->GetPhy ();
  //   }
  // Now end devices are connected to the channel

  /************************
   *  Mobility  *
   ************************/
  MobilityHelper mobility;
//  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0),
//                                 "MinY", DoubleValue (0.0), "DeltaX", DoubleValue (20.0),
//                                 "DeltaY", DoubleValue (10.0), "GridWidth",
//                                 UintegerValue (3), "LayoutType",
//                                 StringValue ("RowFirst"));

  mobility.SetMobilityModel("ns3::WaypointMobilityModel");
  mobility.Install(nodes);

     for (int i = 0; i < 60; ++i)
     {
       Ptr<WaypointMobilityModel> ueWaypointMobility = DynamicCast<WaypointMobilityModel>( nodes.Get(i)->GetObject<MobilityModel>());

       float time = 0.0;

       double w = ((float)rng->GetInteger (54, 66)/(3600 * RADIUS / 1000));

       /*********************************************
        *  Install applications on the end devices  *
        *********************************************/
        Time appStopTime = Seconds (6000);
        PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
        appHelper.SetPeriod (Seconds (10));
        appHelper.SetPacketSize (23);
        Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable> (
        "Min", DoubleValue (0), "Max", DoubleValue (10));
        ApplicationContainer appContainer = appHelper.Install (nodes.Get(i));

        appContainer.Start (Seconds (i * 60));
        appContainer.Stop (appStopTime);


       for (; time <=  TIME * 3600; time += 60)
        {
          if ((time - i* 60) >= 0)
            ueWaypointMobility->AddWaypoint(Waypoint(Seconds(time),Vector(RADIUS*cos(w*(time - i* 60)), RADIUS*sin(w*(time-i*60)),0)));
        }

     }


  ////////////////
  // Simulation //
  ////////////////

     Simulator::Stop (Hours (TIME));

  NS_LOG_INFO("Running simulation...");
  Simulator::Run ();

  Simulator::Destroy ();

  ///////////////////////////
  // Print results to file //
  ///////////////////////////
  NS_LOG_INFO("Computing performance metrics...");

//  LoraPacketTracker &tracker = helper.GetPacketTracker ();
//  std::cout << tracker.CountMacPacketsGlobally (Seconds (0), appStopTime + Hours (1))
//                << std::endl;

  return 0;
}