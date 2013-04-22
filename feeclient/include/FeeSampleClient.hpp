// -*- mode: C++ -*-
#ifndef DCS_FEE_FEE_SAMPLE_CLIENT_HPP
#define DCS_FEE_FEE_SAMPLE_CLIENT_HPP
#ifndef DCS_FEE_FEE_CLIENT_LIB_INTERFACE_HPP
//# include <FeeClientLibInterface.hpp>
#include <feeclient/include/FeeClientLibInterface.hpp>
#endif
#ifndef DCS_FEE_MESSAGE_STRUCT_HPP
# include <MessageStruct.hpp>
#endif
#ifndef __VECTOR__
# include <vector>
#endif
#ifndef __MAP__
# include <map>
#endif
#ifndef __LIST__
# include <list>
#endif
#ifndef __STRING__
# include <string>
#endif

/** @defgroup util Utilities and tools
 */
                                                                                                                                                             
/** @defgroup sample_imp The FeeClient sample implementation
The purpose of providing this sample implementation is twofold.

Firstly, it is a full block i/o implementation (sending and receiving blocks of
data to/from the FeeServer via the FeeClient Library). It is thread-safe and
can communicate with an "unlimited" number of FeeServers simultaneously, even
in the case of the FeeServers replying in a different order than called.

Secondly, if one needs a more tailor made concrete implementation of the
FeeClient Library, it is very convenient to start of with working code to
extend/modify, rather than starting completely from scratch.

It should be stressed that this sample implementation is not part of the
FeeClient library, it is merely bundled with it in the hope that it will be
fund useful.

The sample implementation is quite straight forward to use. One simply passes
the required parameters to the writeReadData(...) function. On return the same
parameters will be populated with return values. For example, if the function
is used to read data from the DCS-card, the data parameter when called will
contain the read-command to the control-engine. On return it will contain the
data read. If one for some reason wants to retain the original data block, it
is the responsibility of the user to make a copy of it before calling the
writeReadData(...) function as it will otherwise be lost. This is true for all
other parameters as well. For the same reason one can not call the function with
static values (typically "flags", "errorCode", "status") as it will try to
write to them.
*/


namespace dcs 
{
  namespace fee 
  {
    class AckData;
    //====================================================================
    /** @struct FeeSampleClient
	@brief Sample client interface for block i/o
	@ingroup feexx
    */
    class FeeSampleClient : public dcs::fee::FeeClientLibInterface
    {
    public:
      /** Constructor */
      FeeSampleClient() {}
      /** Destructor */
      virtual ~FeeSampleClient() {}
    
      /** Type of flags */
      typedef unsigned short FlagBits;
      /** Type of data array */
      typedef std::vector<unsigned int> DataArray;
      //  Buffer to store incoming packets
      typedef std::map<unsigned int,AckData*> AckDataBuffer;
      //  Pool of packets to reuse
      typedef std::list<AckData*> AckDataPool;
      /** Send a command
	  @param serverName Address of server name string.  On return,
	  contains pointer to newly allocated memory with string of
	  responding server name  
	  @param size Input data size (in elements).  On return the size of
	  the returned data (in elements). 
	  @param data Message data.  On return the possible return data.
          The buffer will be extended if the result data needs more space,
          will never get shrinked
	  @param flags Message flags 
	  @param errorCode On return,  error code if any
	  @param status On return, the status
	  @return @c true (1) on success, @c false (0) otherwise */
      bool writeReadData(std::string&  serverName, 
			 size_t&       size, 
			 DataArray&    data, 
			 FlagBits&     flags, 
			 short&        errorCode, 
			 short&        status);
      /** Get an aknowledgement 
	  @param serverName Name of server responding
	  @param size Size of data returned
	  @param data data returned from server. 
	  @param id Identifier obtained by sendCommand
	  @param flags Flags used 
	  @param errorCode Return error code
	  @param status Return status code 
	  @return time left before timeout, or 0 on timeout */
      int getAcknowledge(std::string&  serverName, 
			 size_t&       size, 
			 DataArray&    data, 
			 unsigned int  id, 
			 FlagBits&     flags, 
			 short&        errorCode, 
			 short&        status);
    protected:
      /** Log messages */
      void FeeLogging(int         type, 
		      int         ref, 
		      int         id, 
		      const char* func, 
		      const char* format, 
		      ...);
      /** Handle acknowledgments
	  @param serverName Server 
	  @param size data size 
	  @param data data
	  @param id Id 
	  @param flags Flags
	  @param errorCode Error code
	  @param status status */
      void acknowledgeHandler(char*        serverName, 
			      unsigned int size, 
			      char*        data, 
			      unsigned int id, 
			      FlagBits     flags, 
			      short        errorCode, 
			      short        status);
      /** Handle messages
	  @param msg Message */
      void messageHandler(const dcs::fee::MessageStruct msg);
    private:
      //  Buffer to store incoming packets
      AckDataBuffer fAckDataBuffer;
      //  Pool of packets to reuse
      AckDataPool fAckDataPool;
      /** Current server name */
      std::string curServerName;
      /** Current data size (in bytes) */
      size_t curSize;
      /** Current data */
      DataArray curData;
      /** Current ID */
      unsigned int curId;
      /** Current flags */
      FlagBits     curFlags;
      /** Current error code */
      short        curErrorCode;
      /** Current status */
      short        curStatus;
    };
  }
}

#endif
//
// EOF
//

