/*
 * This script simulates a complex scenario with multiple gateways and end
 * devices. The metric of interest for this script is the throughput of the
 * network.
 */
 
#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/gateway-lorawan-mac.h"
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
#include <algorithm>
#include <ctime>
 
using namespace ns3;
using namespace lorawan;
 
NS_LOG_COMPONENT_DEFINE ("ComplexLorawanNetworkExample");
 
// Network settings
int nPackets = 10000;
int nDevices = 10;
int nGateways = 4;
double radius = 1000;
double lambda = 0.1;
 
// Output control
bool print = true;
 
int
main (int argc, char *argv[])
{
 
  CommandLine cmd;
  cmd.AddValue ("nPackets", "Number of packets for the simulation", nPackets);
  cmd.AddValue ("nDevices", "Number of end devices to include in the simulation", nDevices);
  cmd.AddValue ("radius", "The radius of the area to simulate", radius);
  cmd.AddValue ("lambda", "Packet intensity", lambda);
  cmd.AddValue ("print", "Whether or not to print various informations", print);
  cmd.Parse (argc, argv);
 
  int appPeriodSeconds = 3600; //nDevices/lambda;
  double simulationTime = nPackets/lambda;
 
  std::cout << "lambda=" << lambda << std::endl;
  std::cout << "Gateways=" << nGateways << std::endl;
  std::cout << "Devices=" << nDevices << ", sending packets each " << appPeriodSeconds << " seconds" << std::endl;
  std::cout << "Total simulation time " << simulationTime << " seconds" << std::endl;
 
  // Set up logging
  LogComponentEnable ("ComplexLorawanNetworkExample", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraChannel", LOG_LEVEL_INFO);
  // LogComponentEnable("LoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraInterferenceHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("ClassAEndDeviceLorawanMac", LOG_LEVEL_ALL);
  LogComponentEnable("GatewayLorawanMac", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannel", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraPhyHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMacHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("PeriodicSenderHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("PeriodicSender", LOG_LEVEL_ALL);
  // LogComponentEnable("LorawanMacHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkScheduler", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkServer", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkStatus", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkController", LOG_LEVEL_ALL);
 
  /***********
   *  Setup  *
   ***********/
 
  // Create the time value from the period
  Time appPeriod = Seconds (appPeriodSeconds);
 
  // Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator", "rho", DoubleValue (radius),
                                 "X", DoubleValue (0.0), "Y", DoubleValue (0.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
 
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
  LorawanMacHelper macHelper = LorawanMacHelper ();
 
  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();
  helper.EnablePacketTracking (); 
 
  //Create the NetworkServerHelper
  NetworkServerHelper nsHelper = NetworkServerHelper ();
 
  //Create the ForwarderHelper
  ForwarderHelper forHelper = ForwarderHelper ();
 
  /************************
   *  Create End Devices  *
   ************************/
 
  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (nDevices);
 
  // Assign a mobility model to each node
  mobility.Install (endDevices);
 
  // Make it so that nodes are at a certain height > 0
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      Vector position = mobility->GetPosition ();
      position.z = 1.2;
      mobility->SetPosition (position);
    }
 
  // Create the LoraNetDevices of the end devices
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen =
      CreateObject<LoraDeviceAddressGenerator> (nwkId, nwkAddr);
 
  // Create the LoraNetDevices of the end devices
  macHelper.SetAddressGenerator (addrGen);
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LorawanMacHelper::ED_A);
  macHelper.Set("MType", EnumValue (LorawanMacHeader::CONFIRMED_DATA_UP));
  macHelper.Set("MaxTransmissions", IntegerValue (2));
  helper.Install (phyHelper, macHelper, endDevices);
 
  // Now end devices are connected to the channel
 
  // Connect trace sources
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<LoraNetDevice> loraNetDevice = node->GetDevice (0)->GetObject<LoraNetDevice> ();
      Ptr<LoraPhy> phy = loraNetDevice->GetPhy ();
    }
 
  /*********************
   *  Create Gateways  *
   *********************/
 
  // Create the gateway nodes (allocate them uniformely on the disc)
  NodeContainer gateways;
  gateways.Create (nGateways);
 
  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  // Make it so that nodes are at a certain height > 0
  allocator->Add (Vector (0.0, 0.0, 15.0));
  mobility.SetPositionAllocator (allocator);
  mobility.Install (gateways);
 
  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (LoraPhyHelper::GW);
  macHelper.SetDeviceType (LorawanMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);
 
  /**********************************************
   *  Set up the end device's spreading factor  *
   **********************************************/
 
  //std::vector<double> sfDist = {0, 0, 0, 0, 0, 1}; //min DR0
  //std::vector<double> sfDist = {1, 0, 0, 0, 0, 0};//max DR5
  //std::vector<double> sfDist = {0, 1, 0, 0, 0, 0};//
  //std::vector<double> sfDist = {1./6, 1./6, 1./6, 1./6, 1./6, 1./6};
  std::cout << "DRdistr"<< std::endl;
  std::vector<double> sfDist = {21.66/47.69, 12.38/47.69, 6.99/47.69, 3.90/47.69, 1.76/47.69, 1./47.69};
  std::vector<int> sfs = macHelper.SetSpreadingFactorsGivenDistribution(endDevices, gateways, sfDist);
 
  NS_LOG_DEBUG ("Completed configuration");
 
  /*********************************************
   *  Install applications on the end devices  *
   *********************************************/
 
  Time appStopTime = Seconds (simulationTime);
  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPeriod (Seconds (appPeriodSeconds));
  appHelper.SetPacketSize (51);
  Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable> (
      "Min", DoubleValue (0), "Max", DoubleValue (10));
  ApplicationContainer appContainer = appHelper.Install (endDevices);
 
  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);
 
  /**************************
   *  Create Network Server  *
   ***************************/
 
  // Create the NS node
  NodeContainer networkServer;
  networkServer.Create (1);
 
  // Create a NS for the network
  nsHelper.SetEndDevices (endDevices);
  nsHelper.SetGateways (gateways);
  nsHelper.Install (networkServer);
 
  //Create a forwarder for each gateway
  forHelper.Install (gateways);
 
  ////////////////
  // Simulation //
  ////////////////
 
  Simulator::Stop (appStopTime + Hours (1));
 
  NS_LOG_INFO ("Running simulation...");
  Simulator::Run ();
 
  Simulator::Destroy ();
 
  ///////////////////////////
  // Print results to file //
  ///////////////////////////
  NS_LOG_INFO ("Computing performance metrics...");
 
  LoraPacketTracker &tracker = helper.GetPacketTracker ();
  std::cout << tracker.CountMacPacketsGloballyCpsr(Seconds (0), appStopTime + Hours (1)) << std::endl;
 
  return 0;
}
