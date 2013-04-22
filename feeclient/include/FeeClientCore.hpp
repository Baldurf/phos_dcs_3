#ifndef DCS_FEE_FEE_CLIENT_CORE_HPP
#define DCS_FEE_FEE_CLIENT_CORE_HPP

#include "../dim/dim/dic.hxx"
#include <vector>

#include "FeeAckInfo.hpp"
#include "FeeItemInfo.hpp"
#include "FeeMsgInfo.hpp"
#include "FeeServiceStruct.hpp"

/** @defgroup corelib The FeeClient core
 */

namespace dcs 
{ 
  namespace fee 
  {

    /**
     * Define for the local source of log messages.
     */
#define SOURCE_FEE_CLIENT "FeeClientCore"

    /**
     * Define for state INIT of FeeClientCore.
     */
#define FEE_CLIENT_INIT_STATE 1001

    /**
     * Define for state RUNNING of FeeClientCore.
     */
#define FEE_CLIENT_RUNNING_STATE 1002

    class FeeClientLibInterface;
    class FeeClientLogger;

    /**
     * @class FeeClientCore
     * This class represents the core of the FeeClient in the Fee-Fed
     * library.  It takes care of subscribing to services and handling
     * the communication with the FeeServers.
     *
     * @author Sebastian Bablok, Christian Kofler
     *
     * @date 2003 (changed for LIB 2005-10-05)
     *
     * @ingroup corelib
     */
    class FeeClientCore : public DimClient {
    public:

      /**
       * Constructor of the FeeClient core.  This client holds the
       * connection to the FeeServer.
       *
       * @param interface pointer to the lib interface object
       */
      FeeClientCore(FeeClientLibInterface* interface);

      /**
       * Destructor of the FeeClient core
       */
      virtual ~FeeClientCore();

      /**
       * The infoHandler for incomming data of the FeeServer.
       */
      void infoHandler();

      /**
       * Function to subscribe to certain services.  Gets a vector
       * with char* containing: "ServerName", concatenates
       * "_Acknowledge" and subscribes to it. Same with "_Mesage".
       * additionally gets a vector with char* containing:
       * "ServerName_ServiceName".
       *
       * @param servers pointer to vector with all registered
       * servernames.
       * @param pServices pointer to container of all registered
       * services (including their handler routine for the
       * FeeClientLibInterface)
       *
       * @return amount of subscribed services.
       */
      int subscribeTo(std::vector<char* >* servers,
		      std::vector<FeeServiceStruct* >* pServices);

      /**
       * Getter for the state of the FeeClientCore.
       *
       * @return current state of the FeeClientCore
       * - INIT -> init state, means registering of FeeServers and
       *   their services possible. 
       * - RUNNING -> serving state, is connected to the
       *   FeeServers, no registering of FeeServers or services
       *   possible anymore.
       */
      unsigned int getState();


    private:

      /**
       * Container for pointers of the FeeItemInfo - objects, which
       * represent the monitoring services. After subscribing it
       * should contain all services for monitoring purpose (not the
       * Ack channel).
       */
      std::vector<FeeItemInfo*> mFeeItemInfoCollection;

      /**
       * Container for pointers of the FeeAckInfo - objects, which
       * represent the result channels for commands. After subscribing
       * it should contain all these services.
       */
      std::vector<FeeAckInfo*> mFeeAckInfoCollection;

      /**
       * Container for pointers of the FeeMsgInfo - objects. These
       * channels are used to deliver messages from the
       * FeeServers. After subscribing it should contain all msg
       * channels (to each FeeServer one).
       */
      std::vector<FeeMsgInfo*> mFeeMsgInfoCollection;

      /**
       * The FeeClientLogger object, which takes care of all log
       * messages (remote [from the FeeServers], and local).
       */
      FeeClientLogger* mpLogger;

      /**
       * Pointer to the interface to call the handler functions
       */
      FeeClientLibInterface* mpInterface;

      /**
       * State of the FeeClientCore (INIT, RUNNING).
       */
      unsigned int mState;


    };  // end of class

    inline unsigned int FeeClientCore::getState() {
      return mState;
    };
  } 
} // end of namespace
#endif
