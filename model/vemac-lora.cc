
#include "vemac-lora.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>

using namespace  ns3;





NS_LOG_COMPONENT_DEFINE ("VeMacLora");



VeMacLora::VeMacLora ()
{
  NS_LOG_FUNCTION (this);

//  slots = {'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o'};
  slots[0] = 'o';
  slots[1] = 'o';
  slots[2] = 'o';
  slots[3] = 'o';
  slots[4] = 'o';
  slots[5] = 'o';
  slots[6] = 'o';
  slots[7] = 'o';
  slots[8] = 'o';
  slots[9] = 'o';
  slots[10] = 'o';

}

VeMacLora::~VeMacLora ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

////////////////////////
//  Sending methods   //
////////////////////////


void
VeMacLora::ScheduleTdma (const uint32_t slotNum)
{
  NS_LOG_FUNCTION (slotNum);
  if (slotNum > 9) {
      NS_LOG_WARN("No MAC in slots");
  }

  uint32_t numOfSlotsAllotted = 1;
  while (1) {
      if (slotNum + numOfSlotsAllotted < 10) {
          if (slots[slotNum] == slots[slotNum + numOfSlotsAllotted]) {
              numOfSlotsAllotted ++;
          }
          else
            {
              break;
            }
      }
      else
        {
          break;
        }
  }

  NS_LOG_DEBUG ("Number of slots allotted for this node is: " << numOfSlotsAllotted);
  Time transmissionSlot = Seconds(0.1 * numOfSlotsAllotted);

}

