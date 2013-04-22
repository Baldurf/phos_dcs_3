// -*- mode: C++ -*-
#ifndef ACKDATA_HPP
#define ACKDATA_HPP
#ifndef __VECTOR__
# include <vector>
#endif
#ifndef __STRING__
# include <string>
#endif

namespace dcs
{
  namespace fee
  {
    class AckData
    {
    public:
      /** Type of flags */
      typedef unsigned short FlagBits;
      /** Type of data array */
      typedef std::vector<unsigned int> DataArray;
      /** Default constructor */
      AckData();
      /** Copy constructor 
	  @param newPacket Object to copy from */
      AckData(const AckData& newPacket);
      /** Constructor 
	  @param newServerName Server name that acknowledge 
	  @param newSize       Size of data in acknowledge message
	  @param newData       Data of acknowledge message
	  @param newFlags      Flags 
	  @param newErrorCode  Error code  
	  @param newStatus     Status   */
      AckData(const std::string& newServerName, 
	      const size_t newSize, 
	      const DataArray& newData, 
	      const FlagBits newFlags, 
	      const short newErrorCode, 
	      const short newStatus); 
      /** Constructor 
	  @param newServerName Server name that acknowledge 
	  @param newSize       Size of data in acknowledge message
	  @param newData       Data of acknowledge message
	  @param newFlags      Flags 
	  @param newErrorCode  Error code  
	  @param newStatus     Status */
      AckData(const char* newServerName, 
	      const size_t newSize, 
	      const char* newData, 
	      const FlagBits newFlags, 
	      const short newErrorCode, 
	      const short newStatus);
      /** Destructor 
	  @internal Should be virtual */
      virtual ~AckData();
      // Assignment
      AckData& operator=(const AckData& newPacket);
      /** Set all values
	  @param newServerName Server name that acknowledge 
	  @param newSize       Size of data in acknowledge message
	  @param newData       Data of acknowledge message
	  @param newFlags      Flags 
	  @param newErrorCode  Error code  
	  @param newStatus     Status */
      void set(const char* newServerName, 
	       const size_t newSize, 
	       const char* newData, 
	       const FlagBits newFlags, 
	       const short newErrorCode, 
	       const short newStatus);
      /** Set all values
	  @param newServerName Server name that acknowledge 
	  @param newSize       Size of data in acknowledge message
	  @param newData       Data of acknowledge message
	  @param newFlags      Flags 
	  @param newErrorCode  Error code  
	  @param newStatus     Status   */
      void set(const std::string& newServerName, 
	       const size_t newSize, 
	       const DataArray& newData, 
	       const FlagBits newFlags, 
	       const short newErrorCode, 
	       const short newStatus);
      /** Get all values 
	  @param newServerName On return, the server name 
	  @param newSize       On return, the size of of @a newData
	  @param newData       On return, the data of the acknowledge
	  @param newFlags      On return, the flags
	  @param newErrorCode  On return, the error code
	  @param newStatus     On return, the status */
      void const get(std::string& newServerName, 
	       size_t& newSize, 
	       DataArray& newData, 
	       FlagBits& newFlags, 
	       short& newErrorCode, 
	       short& newStatus);
      /** Get all values 
	  @param newServerName On return, the server name 
	  @param newSize       On return, the size of of @a newData
	  @param newData       On return, the data of the acknowledge
	  @param newFlags      On return, the flags
	  @param newErrorCode  On return, the error code
	  @param newStatus     On return, the status */
      void const get(char*& newServerName, 
	       size_t& newSize, 
	       char*& newData, 
	       FlagBits& newFlags, 
	       short& newErrorCode, 
	       short& newStatus);
    protected:
      /** Server name */
      std::string serverName;
      /** Data size (in bytes) */
      size_t size;
      /** Data */
      DataArray data;
      /** Flags */
      FlagBits flags;
      /** Error code */
      short errorCode;
      /** Status */
      short status;
    private:
    };
  }
}
#endif

//
// EOF
//
