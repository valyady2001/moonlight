#pragma once
#pragma once
#include <sdk/math/vec3.h>
#include <sdk/classes/usercmd.h>
#include <sdk/classes/base_entity.h>

template<typename FuncType>
__forceinline static FuncType call_virtual_test( void* ppClass , int index ) { // lazy piece of shit
	int* pVTable = *( int** ) ppClass;
	int dwAddress = pVTable[ index ];
	return ( FuncType ) ( dwAddress );
}

class player_move_helper {
public:

	bool bFirstRunOfFunctions : 1;
	bool bGameCodeMovedPlayer : 1;
	int nPlayerHandle; // edict index on server, client entity handle on client=
	int nImpulseCommand; // Impulse command issued.
	vec3_t vecViewAngles; // Command view angles (local space)
	vec3_t vecAbsViewAngles; // Command view angles (world space)
	int nButtons; // Attack buttons.
	int nOldButtons; // From host_client->oldbuttons;
	float flForwardMove;
	float flSideMove;
	float flUpMove;
	float flMaxSpeed;
	float flClientMaxSpeed;
	vec3_t vecVelocity; // edict::velocity // Current movement direction.
	vec3_t vecAngles; // edict::angles
	vec3_t vecOldAngles;
	float outStepHeight; // how much you climbed this move
	vec3_t outWishVel; // This is where you tried
	vec3_t outJumpVel; // This is your jump velocity
	vec3_t vecConstraintCenter;
	float flConstraintRadius;
	float flConstraintWidth;
	float flConstraintSpeedFactor;
	float flUnknown[ 5 ];
	vec3_t vecAbsOrigin;

	virtual	void _vpad( ) = 0;
	virtual void set_host( base_entity* host ) = 0;
	virtual void pad01( ) = 0;
	virtual void pad02( ) = 0;
	virtual void process_impacts( ) = 0;
};

class player_move_data {
public:
	bool    bFirstRunOfFunctions : 1;
	bool    bGameCodeMovedPlayer : 1;
	int     nPlayerHandle;        // edict index on server, client entity handle on client=
	int     nImpulseCommand;      // Impulse command issued.
	vec3_t    vecViewAngles;        // Command view angles (local space)
	vec3_t  vecAbsViewAngles;     // Command view angles (world space)
	int     nButtons;             // Attack buttons.
	int     nOldButtons;          // From host_client->oldbuttons;
	float   flForwardMove;
	float   flSideMove;
	float   flUpMove;
	float   flMaxSpeed;
	float   flClientMaxSpeed;
	vec3_t  vecVelocity;          // edict::velocity        // Current movement direction.
	vec3_t  vecAngles;            // edict::angles
	vec3_t  vecOldAngles;
	float   outStepHeight;        // how much you climbed this move
	vec3_t  outWishVel;           // This is where you tried
	vec3_t  outJumpVel;           // This is your jump velocity
	vec3_t  vecConstraintCenter;
	float   flConstraintRadius;
	float   flConstraintWidth;
	float   flConstraintSpeedFactor;
	float   flUnknown[ 5 ];
	vec3_t  vecAbsOrigin;
};

class virtual_game_movement {
public:
	virtual			~virtual_game_movement( void ) { }

	virtual void	          process_movement( base_entity *pPlayer , player_move_data *pMove ) = 0;
	virtual void	          reset( void ) = 0;
	virtual void	          start_track_prediction_errors( base_entity *pPlayer ) = 0;
	virtual void	          finish_track_prediction_errors( base_entity *pPlayer ) = 0;
	virtual void	          DiffPrint( char const *fmt , ... ) = 0;
	virtual vec3_t const&	  GetPlayerMins( bool ducked ) const = 0;
	virtual vec3_t const&	  GetPlayerMaxs( bool ducked ) const = 0;
	virtual vec3_t const&   GetPlayerViewOffset( bool ducked ) const = 0;
	virtual bool		        IsMovingPlayerStuck( void ) const = 0;
	virtual base_entity*   GetMovingPlayer( void ) const = 0;
	virtual void		        UnblockPusher( base_entity *pPlayer , base_entity *pPusher ) = 0;
	virtual void            SetupMovementBounds( player_move_data *pMove ) = 0;
};

class player_game_movement
	: public virtual_game_movement {
public:
	virtual ~player_game_movement( void ) { }
};

class player_prediction {
public:
	void run_command( base_entity *player , usercmd *ucmd , player_move_helper *moveHelper ) {
		typedef void( __thiscall* oRunCommand )( void* , base_entity* , usercmd* , player_move_helper* );
		return call_virtual_test<oRunCommand>( this , 19 )( this , player , ucmd , moveHelper );
	}

	void setup_move( base_entity *player , usercmd *ucmd , player_move_helper *moveHelper , void* pMoveData ) {
		typedef void( __thiscall* oSetupMove )( void* , base_entity* , usercmd* , player_move_helper* , void* );
		return call_virtual_test<oSetupMove>( this , 20 )( this , player , ucmd , moveHelper , pMoveData );
	}
	void update( int start_frame , bool valid_frame , int inc_ack , int out_cmd ) {
		using fn = void( __thiscall* )( void* , int , bool , int , int );
		vfunc< fn >( this , 3 )( this , start_frame , valid_frame , inc_ack , out_cmd );
	}


	void finish_move( base_entity *player , usercmd *ucmd , void*pMoveData ) {
		typedef void( __thiscall* oFinishMove )( void* , base_entity* , usercmd* , void* );
		return call_virtual_test<oFinishMove>( this , 21 )( this , player , ucmd , pMoveData );
	}

	void set_local_viewangles( vec3_t angle ) {
		typedef void( __thiscall * original_fn )( void* , vec3_t );
		return call_virtual_test<original_fn>( this , 13 )( this , angle );
	}
	void check_moving_ground( base_entity* player , double frametime ) {
		using fn = void( __thiscall* )( void* , base_entity* , double );
		vfunc< fn >( this , 18 )( this , player , frametime );
	}
	PAD( 0x4 );
	int32_t m_last_ground;				// 0x0004
	bool    m_in_prediction;			// 0x0008
	bool    m_first_time_predicted;		// 0x0009
	bool    m_engine_paused;			// 0x000A
	bool    m_old_cl_predict_value;		// 0x000B
	int32_t m_previous_startframe;		// 0x000C
	int32_t m_commands_predicted;		// 0x0010
	PAD( 0x38 );						// 0x0014
	float   m_backup_realtime;			// 0x004C
	PAD( 0xC );							// 0x0050
	float   m_backup_curtime;			// 0x005C
	PAD( 0xC );							// 0x0060
	float   m_backup_interval;			// 0x006C


};