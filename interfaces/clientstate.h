#pragma once
#pragma once
#include <sdk/math/vec3.h>
#include <interfaces/client.h>
#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out


class bf_write;
class bf_read;
class net_channel;
class i_net_message
{
public:
	virtual	~i_net_message( ) {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	set_netchannel( net_channel* netchan ) = 0; // netchannel this message is from/for
	virtual void	set_reliable( bool state ) = 0;	// set to true if it's a reliable message

	virtual bool	process( void ) = 0; // calles the recently set handler to process this message

	virtual	bool	read_from_buffer( bf_read& buffer ) = 0; // returns true if parsing was ok
	virtual	bool	write_to_buffer( bf_write& buffer ) = 0;	// returns true if writing was ok

	virtual bool	is_reliable( void ) const = 0;  // true, if message needs reliable handling

	virtual int				get_type( void ) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				get_group( void ) const = 0;	// returns net message group of this message
	virtual const char* getname( void ) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual net_channel* get_netchannel( void ) const = 0;
	virtual const char* to_string( void ) const = 0; // returns a human readable string about message content
};

class i_net_channel_info
{
public:

	enum
	{
		GENERIC = 0 ,	// must be first and is default group
		LOCALPLAYER ,	// bytes for local player entity update
		OTHERPLAYERS ,	// bytes for other players update
		ENTITIES ,		// all other entity bytes
		SOUNDS ,			// game sounds
		EVENTS ,			// event messages
		USERMESSAGES ,	// user messages
		ENTMESSAGES ,	// entity messages
		VOICE ,			// voice data
		STRINGTABLE ,	// a stringtable update
		MOVE ,			// client move cmds
		STRINGCMD ,		// string command
		SIGNON ,			// various signondata
		TOTAL ,			// must be last and is not a real group
	};

	virtual const char* get_name( void ) const = 0;	// get channel name
	virtual const char* get_address( void ) const = 0; // get channel IP address as string
	virtual float		get_time( void ) const = 0;	// current net time
	virtual float		get_time_connected( void ) const = 0;	// get connection time in seconds
	virtual int			get_buffer_size( void ) const = 0;	// netchannel packet history size
	virtual int			get_data_rate( void ) const = 0; // send data rate in byte/sec

	virtual bool		is_loop_back( void ) const = 0;	// true if loopback channel
	virtual bool		is_timing_out( void ) const = 0;	// true if timing out
	virtual bool		is_play_back( void ) const = 0;	// true if demo playback

	virtual float		get_latency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		get_average_latency( int flow ) const = 0; // average packet latency in seconds
	virtual float		get_average_loss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		get_average_choke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		get_average_data( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		get_average_packets( int flow ) const = 0; // avg packets/sec
	virtual int			get_total_data( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			get_sequence_number( int flow ) const = 0;	// last send seq number
	virtual bool		is_valid_packet( int flow , int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		get_packet_time( int flow , int frame_number ) const = 0; // time when packet was send
	virtual int			get_packet_bytes( int flow , int frame_number , int group ) const = 0; // group size of this packet
	virtual bool		get_stream_progress( int flow , int* received , int* total ) const = 0;  // TCP progress if transmitting
	virtual float		get_since_last_time_recieved( void ) const = 0;	// get time since last recieved packet in seconds
	virtual	float		get_command_interpolation_ammount( int flow , int frame_number ) const = 0;
	virtual void		get_packet_response_latency( int flow , int frame_number , int* pnLatencyMsecs , int* pnChoke ) const = 0;
	virtual void		get_remote_framerate( float* pflFrameTime , float* pflFrameTimeStdDeviation ) const = 0;

	virtual float		get_timeout_seconds( ) const = 0;
};

class CClockDriftMgr
{
public:
	float m_ClockOffsets[ 16 ];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050
#include <string>
class bf_read
{
public:
	uintptr_t base_address;
	uintptr_t cur_offset;

	bf_read( uintptr_t addr )
	{
		base_address = addr;
		cur_offset = 0;
	}

	void SetOffset( uintptr_t offset )
	{
		cur_offset = offset;
	}

	void Skip( uintptr_t length )
	{
		cur_offset += length;
	}

	int ReadByte( )
	{
		auto val = *reinterpret_cast< char* >( base_address + cur_offset );
		++cur_offset;
		return val;
	}

	bool ReadBool( )
	{
		auto val = *reinterpret_cast< bool* >( base_address + cur_offset );
		++cur_offset;
		return val;
	}

	std::string ReadString( )
	{
		char buffer[ 256 ];
		auto str_length = *reinterpret_cast< char* >( base_address + cur_offset );
		++cur_offset;
		memcpy( buffer , reinterpret_cast< void* >( base_address + cur_offset ) , str_length > 255 ? 255 : str_length );
		buffer[ str_length > 255 ? 255 : str_length ] = '\0';
		cur_offset += str_length + 1;
		return std::string( buffer );
	}
};
class bf_write {
public:
	unsigned char* data;
	int data_bytes;
	int data_bits;
	int cur_bit;
	bool overflow;
	bool assert_on_overflow;
	const char* debug_name;

	void start_writing( void* m_data , int m_bytes , int m_start_bit = 0 , int m_bits = -1 ) {
		m_bytes &= ~3;

		data = ( unsigned char* ) m_data;
		data_bytes = m_bytes;

		if ( m_bits == -1 ) {
			data_bits = m_bytes << 3;
		}
		else {
			data_bits = m_bits;
		}

		cur_bit = m_start_bit;
		overflow = false;
	}

	bf_write( ) {
		data = NULL;
		data_bytes = 0;
		data_bits = -1;
		cur_bit = 0;
		overflow = false;
		assert_on_overflow = true;
		debug_name = NULL;
	}

	bf_write( void* m_data , int m_bytes , int m_bits = -1 ) {
		assert_on_overflow = true;
		debug_name = NULL;
		start_writing( m_data , m_bytes , 0 , m_bits );
	}

	bf_write( const char* m_debug_name , void* m_data , int m_bytes , int m_bits = -1 ) {
		assert_on_overflow = true;
		debug_name = m_debug_name;
		start_writing( m_data , m_bytes , 0 , m_bits );
	}
};

#include <string>

template<typename T>
class CNetMessagePB : public i_net_message , public T {};

class CCLCMsg_Move {
private:
	char __PAD0[ 0x8 ];
public:
	int numBackupCommands; // 0x8
	int numNewCommands;// 0xC
	std::string* m_data;   // 0x10 std::string
	void* m_DataIn;      // 0x14
	bf_write m_DataOut;    // 0x38
};// size: 0x50

using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;


class net_channel
{
public:
	char pad_0000[ 20 ];           //0x0000
	bool processing_messages;  //0x0014
	bool should_delete;        //0x0015
	char pad_0016[ 2 ];            //0x0016
	int32_t out_sequencenr;    //0x0018 last send outgoing sequence number
	int32_t in_sequencenr;     //0x001c last received incoming sequnec number
	int32_t out_sequencenrack; //0x0020 last received acknowledge outgoing sequnce number
	int32_t out_reliable_state; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t in_reliable_state;  //0x0028 state of incoming reliable data
	int32_t choked_packets;    //0x002c number of choked packets
	char pad_0030[ 1044 ];         //0x0030

	bool transmit( bool onlyReliable )
	{
		using original_fn = bool( __thiscall* )( void* , bool );
		return ( *( original_fn** ) this )[ 49 ]( this , onlyReliable );;
	}

	int send_datagram( bf_write* data = nullptr )
	{
		using original_fn = int( __thiscall* )( void* , bf_write* );
		return ( *( original_fn** ) this )[ 46 ]( this , data );
	}
	void sendmsg( CCLCMsg_Move& msg , bool fr = false , bool vc = false )
	{
		using netmsg_fn = void( __thiscall* )( void* , CCLCMsg_Move& , bool , bool );
		return ( *( netmsg_fn** ) this )[ 40 ]( this , msg , fr , vc );
	}
};

typedef struct netpacket_s
{
	void*		 from;		// sender address
	int				source;		// received source 
	double			received;	// received time
	unsigned char* data;		// pointer to raw packet data
	bf_read			message;	// easy bitbuf data access
	int				size;		// size in bytes
	int				wiresize;   // size in bytes before decompression
	bool			stream;		// was send as stream
	struct netpacket_s* pNext;	// for internal use, should be NULL in public
} netpacket_t;


class event_delay
{
public:
	short				sClassID;		// 0x00 // 0 implies not in use
	float				flFireDelay;	// 0x02 // if non-zero, the delay time when the event should be fired ( fixed up on the client )
	const void* pSendTable;		// 0x06
	const client_class_t* pClientClass;	// 0x0A // clienclass pointer
	void* pData;			// 0x0E // raw event data
	std::intptr_t		iPackedBits;	// 0x12
	int					iFlags;			// 0x16
	std::byte			pad0[ 0x16 ];		// 0x1A
	event_delay* next;
}; // Size: 0x30


enum sign_state_t : int
{
	signonstate_none = 0 ,
	signonstate_challenge = 1 ,
	signonstate_connected = 2 ,
	signonstate_new = 3 ,
	signonstate_prespawn = 4 ,
	signonstate_spawn = 5 ,
	signonstate_full = 6 ,
	signonstate_changelevel = 7
};

class clientstate
{
public:
	char            pad0[ 156 ];
	net_channel* m_net_channel;
	int                m_nChallengeNr;
	char            pad1[ 4 ];
	double            m_connect_time;
	int                m_retry_number;
	char            pad2[ 84 ];
	int                m_nSignonState;
	char            pad3[ 4 ];
	double            m_flNextCmdTime;
	int                m_nServerCount;
	int                m_current_sequence;
	char            pad4[ 8 ];

	struct {
		float        clock_offsets[ 16 ];
		int            cur_clock_offset;
		int            m_server_tick;
		int            m_client_tick;
	} clock_drift_mgr;

	int                m_delta_tick;
	char            pad5[ 4 ];
	int                m_nViewEntity;
	int                m_nPlayerSlot;
	bool            m_bPaused;
	char            pad6[ 3 ];
	char            m_szLevelName[ 260 ];
	char            m_szLevelNameShort[ 40 ];
	char            pad7[ 212 ];
	int                m_nMaxClients;
	char            pad8[ 18836 ];
	int                oldtickcount;
	float            m_tickRemainder;
	float            m_frameTime;
	int                m_last_outgoing_command;
	int                m_choked_commands;
	int                m_last_command_ack;
	int                m_last_server_tick;
	int                m_command_ack;
	int                m_sound_sequence;
	int                m_last_progress_percent;
	bool            m_is_hltv;
	char            pad9[ 75 ];
	vec3_t            m_viewangles;
	char            pad10[ 204 ];
	event_delay* m_events;

}; // Size: 0x4Eze: 0x4E1C


