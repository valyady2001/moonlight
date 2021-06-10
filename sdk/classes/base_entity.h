#pragma once
#include <sdk/defines.h>
#include <sdk/math/vec3.h>
#include <interfaces/model_info.h>
#include <sdk/utils/netvars.h>
#include "../utils/utilities.h"
#include "collideable.h"
#include <interfaces/client.h>
#include <array>


class base_entity;
class c_weapon;
class animstate;
class player_resource;
class vec3_t;
class vec2_t;
class IRecipientFilter;
class CBreakableSurface;
class usercmd;

enum animstate_pose_param_idx_t
{
	PLAYER_POSE_PARAM_FIRST = 0 ,
	PLAYER_POSE_PARAM_LEAN_YAW = PLAYER_POSE_PARAM_FIRST ,
	PLAYER_POSE_PARAM_SPEED ,
	PLAYER_POSE_PARAM_LADDER_SPEED ,
	PLAYER_POSE_PARAM_LADDER_YAW ,
	PLAYER_POSE_PARAM_MOVE_YAW ,
	PLAYER_POSE_PARAM_RUN ,
	PLAYER_POSE_PARAM_BODY_YAW ,
	PLAYER_POSE_PARAM_BODY_PITCH ,
	PLAYER_POSE_PARAM_DEATH_YAW ,
	PLAYER_POSE_PARAM_STAND ,
	PLAYER_POSE_PARAM_JUMP_FALL ,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_IDLE ,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_IDLE ,
	PLAYER_POSE_PARAM_STRAFE_DIR ,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK ,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN ,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK ,
	PLAYER_POSE_PARAM_MOVE_BLEND_WALK ,
	PLAYER_POSE_PARAM_MOVE_BLEND_RUN ,
	PLAYER_POSE_PARAM_MOVE_BLEND_CROUCH_WALK ,
	//PLAYER_POSE_PARAM_STRAFE_CROSS,
	PLAYER_POSE_PARAM_COUNT ,
};


struct animstate_pose_param_cache {
	std::uint8_t pad_0x0[ 0x4 ]; //0x0
	std::uint32_t m_idx; //0x4
	char* m_name;
	void set_value( base_entity* e , float val );
	//0x8
};

enum data_update_type_t {
	data_update_created = 0 ,
	data_update_datatable_changed
};

class animstate {
public:
	std::uint8_t pad_0x0000[ 0x4 ]; //0x0000
	bool force_update; //0x0005 
	std::uint8_t pad_0x0006[ 0x5A ]; //0x0006
	base_entity* entity; //0x0060 
	c_weapon* weapon; //0x0064 
	c_weapon* last_weapon; //0x0068 
	float last_clientside_anim_update; //0x006C 
	std::uint32_t last_clientside_anim_framecount; //0x0070 
	float last_clientside_anim_update_time_delta; //0x0074 
	float eye_yaw; //0x0078 
	float pitch; //0x007C
	float abs_yaw; //0x0080 
	float feet_yaw; //0x0084 
	float body_yaw; //0x0088 
	float body_yaw_clamped; //0x008C 
	float feet_vel_dir_delta; //0x0090 
	std::uint8_t pad_0x0094[ 0x4 ]; //0x0094
	float feet_cycle; //0x0098 
	float feet_yaw_rate; //0x009C 
	std::uint8_t pad_0x00A0[ 0x4 ]; //0x00A0
	float duck_amount; //0x00A4 
	float landing_duck_additive; //0x00A8 
	std::uint8_t pad_0x00AC[ 0x4 ]; //0x00AC
	vec3_t origin; //0x00B0 
	vec3_t old_origin; //0x00BC 
	vec2_t vel2d; //0x00C8 
	std::uint8_t pad_0x00D0[ 0x10 ]; //0x00D0
	vec2_t last_accelerating_vel; //0x00E0 
	std::uint8_t pad_0x00E8[ 0x4 ]; //0x00E8
	float speed2d; //0x00EC 
	float up_vel; //0x00F0 
	float speed_normalized; //0x00F4 
	float run_speed; //0x00F8 
	float duck_speed; //0x00FC 
	float time_since_move; //0x0100 
	float time_since_stop; //0x0104 
	bool on_ground; //0x0108 
	bool hit_ground; //0x0109 
	std::uint8_t pad_0x010A[ 0x4 ]; //0x010A
	float time_in_air; //0x0110 
	std::uint8_t pad_0x0114[ 0x6 ]; //0x0114
	float ground_fraction; //0x011C 
	std::uint8_t pad_0x0120[ 0x2 ]; //0x0120
	float unk_fraction; //0x0124 
	std::uint8_t pad_0x0128[ 0xC ]; //0x0128
	bool moving; //0x0134
	std::uint8_t pad_0x0135[ 0x7B ]; //0x0135
	animstate_pose_param_cache lean_yaw_pose; //0x1B0
	animstate_pose_param_cache speed_pose; //0x01BC
	animstate_pose_param_cache ladder_speed_pose; //0x01C8
	animstate_pose_param_cache ladder_yaw_pose; //0x01D4
	animstate_pose_param_cache move_yaw_pose; //0x01E0
	animstate_pose_param_cache run_pose; //0x01EC 
	animstate_pose_param_cache body_yaw_pose; //0x01F8
	animstate_pose_param_cache body_pitch_pose; //0x0204
	animstate_pose_param_cache dead_yaw_pose; //0x0210
	animstate_pose_param_cache stand_pose; //0x021C
	animstate_pose_param_cache jump_fall_pose; //0x0228
	animstate_pose_param_cache aim_blend_stand_idle_pose; //0x0234
	animstate_pose_param_cache aim_blend_crouch_idle_pose; //0x0240
	animstate_pose_param_cache strafe_yaw_pose; //0x024C
	animstate_pose_param_cache aim_blend_stand_walk_pose; //0x0258
	animstate_pose_param_cache aim_blend_stand_run_pose; //0x0264
	animstate_pose_param_cache aim_blend_crouch_walk_pose; //0x0270
	animstate_pose_param_cache move_blend_walk_pose; //0x027C
	animstate_pose_param_cache move_blend_run_pose; //0x0288
	animstate_pose_param_cache move_blend_crouch_pose; //0x0294
	std::uint8_t pad_0x02A0[ 0x4 ]; //0x02A0
	float vel_unk; //0x02A4 
	std::uint8_t pad_0x02A8[ 0x86 ]; //0x02A8
	float min_yaw; //0x0330 
	float max_yaw; //0x0334 
	float max_pitch; //0x0338 
	float min_pitch; //0x033C

	void update( const vec3_t& ang );

	bool& m_smooth_height_valid( )
	{
		return *reinterpret_cast< bool* >( uintptr_t( this ) + 0x328 );
	}
};

struct animlayer {
	PAD( 20 );
	int	order;
	int	sequence;
	float previous_cycle;
	float weight;
	float weight_delta_rate;
	float playback_rate;
	float cycle;
	void* owner;
	PAD( 4 );
};

class c_bone_accessor {
public:
	int get_readable( ) {
		return m_readable;
	}

	void set_readable( int flags ) {
		m_readable = flags;
	}

	int get_writeable( ) {
		return m_writeable;
	}

	void set_writeable( int flags ) {
		m_writeable = flags;
	}

	const matrix_t& get_bone( int bone ) const {
		return m_bones[ bone ];
	}

	matrix_t& get_bone_for_write( int bone ) {
		return m_bones[ bone ];
	}

	std::array< matrix_t , 128 >& get_bone_arr_for_write( ) const {
		return m_bones;
	};

	const void* m_animating;
	std::array< matrix_t , 128 >& m_bones;
	int m_readable;
	int m_writeable;
};
class BoneArray : public matrix_t {
public:
	bool get_bone( vec3_t& out , int bone = 0 ) {
		if ( bone < 0 || bone >= 128 )
			return false;

		matrix_t* bone_matrix = &this[ bone ];

		if ( !bone_matrix )
			return false;

		out = { bone_matrix->m_values[ 0 ][ 3 ], bone_matrix->m_values[ 1 ][ 3 ], bone_matrix->m_values[ 2 ][ 3 ] };

		return true;
	}
};

class CBoneAccessor {
public:
	void* m_pAnimating;
	BoneArray* m_pBones;
	int        m_ReadableBones;
	int        m_WritableBones;
};


class CBoneCache {
public:
	BoneArray* m_pCachedBones;
	PAD( 0x8 );
	int        m_CachedBoneCount;
};

enum InvalidatePhysicsBits_t : int {
	POSITION_CHANGED = 0x1 ,
	ANGLES_CHANGED = 0x2 ,
	VELOCITY_CHANGED = 0x4 ,
	ANIMATION_CHANGED = 0x8 ,
	BOUNDS_CHANGED = 0x10 ,
	SEQUENCE_CHANGED = 0x20
};

struct varmapentry_t {
	std::uint16_t m_type;
	std::uint16_t m_needstointerpolate;
	void* data;
	void* watcher;
};

struct varmapping_t {
	varmapentry_t* m_entries;
	int m_interpolatedentries;
	float m_lastinterptime;
};


class Address
{
protected:
	uintptr_t m_addr;
public:
	__forceinline  Address( ) : m_addr{} {};
	__forceinline ~Address( ) {};

	// ctors.
	__forceinline Address( uintptr_t a ) : m_addr{ a } {}
	__forceinline Address( const void* a ) : m_addr{ ( uintptr_t ) a } {}

	// arithmetic operators for native types.
	__forceinline operator uintptr_t( ) { return m_addr; }
	__forceinline operator void* ( ) { return ( void* ) m_addr; }
	__forceinline operator const void* ( ) { return ( const void* ) m_addr; }

	// is-equals-operator.
	__forceinline bool operator==( Address a ) const {
		return as< uintptr_t >( ) == a.as< uintptr_t >( );
	}
	__forceinline bool operator!=( Address a ) const {
		return as< uintptr_t >( ) != a.as< uintptr_t >( );
	}

	template< typename t = Address >
	__forceinline t as( ) const {
		return ( m_addr ) ? ( t ) m_addr : t{};
	}

	template< typename t = Address >
	__forceinline t as( size_t offset ) const {
		return ( m_addr ) ? ( t ) ( m_addr + offset ) : t{};
	}

	template< typename t = Address >
	__forceinline t as( ptrdiff_t offset ) const {
		return ( m_addr ) ? ( t ) ( m_addr + offset ) : t{};
	}

	// add offset and dereference.
	template< typename t = Address >
	__forceinline t at( size_t offset ) const {
		return ( m_addr ) ? *( t* ) ( m_addr + offset ) : t{};
	}

	template< typename t = Address >
	__forceinline t at( ptrdiff_t offset ) const {
		return ( m_addr ) ? *( t* ) ( m_addr + offset ) : t{};
	}

	template< typename t = Address >
	__forceinline t to( ) const {
		return *( t* ) m_addr;
	}
};
class ik_context
{
public:
	void init( studiohdr_t* hdr , vec3_t& angles , vec3_t& origin , float curtime , int framecount , int boneMask ) {
		static const auto ik_init_address = utilities::find_sig( "client.dll" , "55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D 8F" );
		reinterpret_cast< void( __thiscall* )( ik_context* , studiohdr_t* , vec3_t& , vec3_t& , float , int , int ) >( ik_init_address )( this , hdr , angles , origin , curtime , framecount , boneMask );
	}

	void update_targets( vec3_t* pos , quaternion* q , matrix_t* bone_array , byte* computed ) {
		static const auto update_targets_address = utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 81 EC ? ? ? ? 33 D2" );
		reinterpret_cast< void( __thiscall* )( ik_context* , vec3_t* , quaternion* , matrix_t* , byte* ) >( update_targets_address )( this , pos , q , bone_array , computed );
	}

	void solve_dependencies( vec3_t* pos , quaternion* q , matrix_t* bone_array , byte* computed ) {
		static const auto solve_dependencies_address = utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 81 EC ? ? ? ? 8B 81" );
		reinterpret_cast< void( __thiscall* )( ik_context* , vec3_t* , quaternion* , matrix_t* , byte* ) >( solve_dependencies_address )( this , pos , q , bone_array , computed );
	}

	void clear_targets( )
	{
		auto v56 = 0;
		if ( *( int* ) ( ( DWORD ) this + 4080 ) > 0 )
		{
			auto v57 = ( int* ) ( ( DWORD ) this + 208 );
			do
			{
				*v57 = -9999;
				v57 += 85;
				++v56;
			} while ( v56 < *( int* ) ( ( DWORD ) this + 4080 ) );
		}
	}
};

class base_entity
{
private:
	template< typename t >
	__forceinline void set( size_t offset , const t& val ) {
		*( t* ) ( ( uintptr_t ) this + offset ) = val;
	}
	template< typename t >
	__forceinline t& get( size_t offset ) {
		return *( t* ) ( ( uintptr_t ) this + offset );
	}

	template < typename c = Address  >
	__forceinline  c get_method( Address this_ptr , size_t index ) {
		return ( c ) this_ptr.to< c* >( )[ index ];
	}

public:
	netvar( int , team , "DT_BaseEntity" , "m_iTeamNum" );
	netvar( bool , spotted , "DT_BaseEntity" , "m_bSpotted" );
	netvar( float_t , throw_time , "DT_BaseCSGrenade" , "m_fThrowTime" );
	netvar( vec3_t , view_offset , "DT_BasePlayer" , "m_vecViewOffset[0]" );
	netvar( int , armor , "DT_CSPlayer" , "m_ArmorValue" );
	netvar( float , simtime , "DT_BaseEntity" , "m_flSimulationTime" );
	//netvar( bool , client_animations , "DT_BaseAnimating" , "m_bClientSideAnimation" );
	netvar( int , hitbox_set , "DT_BaseAnimating" , "m_nHitboxSet" );
	netvar( float , duck_speed , "DT_BasePlayer" , "m_flDuckSpeed" );
	netvar( float , duck_amount , "DT_BasePlayer" , "m_flDuckAmount" );
	netvar( float , get_health_boost_time , "DT_CSPlayer" , "m_flHealthShotBoostExpirationTime" );
	netvar( std::uint32_t , weapon_handle , "DT_BaseCombatCharacter" , "m_hActiveWeapon" );
	netvar( int , money , "DT_CSPlayer" , "m_iAccount" );
	netvar( vec3_t , angles , "DT_CSPlayer" , "m_angEyeAngles" );
	netvar( int , flags , "DT_CSPlayer" , "m_fFlags" );
	netvar( bool , is_scoped , "DT_CSPlayer" , "m_bIsScoped" );
	netvar( float , flash_duration , "DT_CSPlayer" , "m_flFlashDuration" );
	netvar( float , flash_alpha , "DT_CSPlayer" , "m_flFlashMaxAlpha" );
	netvar( float , m_flNextAttack , "DT_BaseCombatCharacter" , "m_flNextAttack" );
	netvar( float , velocity_modifier , "DT_BaseEntity" , "m_flVelocityModifier" );
	netvar( int , model_index , "DT_BaseEntity" , "m_nModelIndex" );

	offset( bool , use_new_animstate , 0x3AC8 );
	offset( float , spawn_time , 0xA370 );
	offset( int , eflags , 0xE8 );
	offset( bool , has_helmet , 0xB36C );
	offset( float , lower_body_yaw , 0x3A90 );
	offset( int , tick_base , 0x3430 );
	offset( int , health , 0x100 );
	offset( vec3_t , velocity , 0x114 );
	offset( vec3_t , unk , 0x118 );
	offset( vec3_t , abs_velocity , 0x94 );
	offset( bool , dormant , 0xED );
	offset( bool , has_defuser , 0xB388 );
	offset( vec3_t , aim_punch_angle , 0x302C );
	offset( vec3_t , aim_punch_angle_vel , 0x3038 );
	offset( vec3_t , punch_angle , 0x3020 );
	offset( vec3_t , origin , 0x138 );
	offset( vec3_t , old_origin , 0x3A8 );
	offset( std::uint32_t , num_overlays , 0x298C );
	offset( c_bone_accessor , bone_accessor , 0x290C );
	offset( bool , jiggle_bones_enabled , 0x292C );
	offset( int , effects , 0xF0 );
	offset( int , GetTakeDamage , 0x280 );
	offset( int , m_iOcclusionFlags , 0xA28 );
	offset( int , m_iOcclusionFramecount , 0xA30 );
	offset( uint32_t , GetReadableBones , 0x26AC );
	offset( uint32_t , GetWriteableBones , 0x26B0 );
	offset( matrix_t* , GetBoneArrayForWrite , 0x26A8 );
	offset( matrix_t* , bone_cache2 , 0x2910 );
	offset( bool , immune , 0x3944 );
	netvar( bool , is_ghost , "DT_CSPlayer" , "m_bIsPlayerGhost" );
	netvar( bool , is_broken , "DT_BreakableSurface" , "m_bIsBroken" );
	netvar( std::uint32_t , get_viewmodel , "DT_BasePlayer" , "m_hViewModel[0]" );
	netvar( int32_t , viewmodel_model_index , "DT_BaseViewModel" , "m_nModelIndex" );
	netvar( int32_t , viewmodel_viewmodel_model_index , "DT_BaseViewModel" , "m_nViewModelIndex" );
	netvar( std::uint32_t , viewmodel_weapon , "DT_BaseViewModel" , "m_hWeapon" );
	netvar( std::uint32_t , viewmodel_owner , "DT_BaseViewModel" , "m_hOwner" );
	netvar( bool , heavy_armor , "DT_CSPlayer" , "m_bHasHeavyArmor" );
	netvar( int , GetCollisionGroup , "DT_BaseEntity" , "m_CollisionGroup" );
	netvar( int , sequence , "DT_PredictedViewModel" , "m_nSequence" );
	netvar( int , cycle , "DT_CSPlayer" , "m_flCycle" );

	void update_ik_locks( float curtime ) {
		using o_fn = void( __thiscall* )( void* , float );
		get_method<o_fn>( this , 191 )( this , curtime );
	}

	void calculate_ik_locks( float curtime ) {
		using o_fn = void( __thiscall* )( void* , float );
		get_method<o_fn>( this , 192 )( this , curtime );
	}
	ik_context* get_ik_context( ) {
		return *( ik_context** ) ( ( uintptr_t ) this + 9836 + 0x4 );
	}
	std::array<int , 5>& m_vecPlayerPatchEconIndices( ) {
		return *( std::array<int , 5>* )( ( uintptr_t ) this + 0xB970 );
	}

	void BuildTransformations( studiohdr_t* hdr , vec3_t* pos , quaternion* q , const matrix_t& transform , int mask , uint8_t* computed ) {
		using BuildTransformations_t = void( __thiscall* )( decltype( this ) , studiohdr_t* , vec3_t* , quaternion* , matrix_t const& , int , uint8_t* );
		return get_method< BuildTransformations_t >( this , 189 )( this , hdr , pos , q , transform , mask , computed );
	}

	void StandardBlendingRules( studiohdr_t* hdr , vec3_t* pos , quaternion* q , float time , int mask ) {
		using StandardBlendingRules_t = void( __thiscall* )( decltype( this ) , studiohdr_t* , vec3_t* , quaternion* , float , int );
		return get_method< StandardBlendingRules_t >( this , 205 )( this , hdr , pos , q , time , mask );
	}
	studiohdr_t* get_model_ptr( ) {
		return *( studiohdr_t** ) ( ( uintptr_t ) this + 0x294C );
	}
	bool& client_animations( );
	float GetAnimationTime( )
	{
		return *reinterpret_cast< float* >( ( DWORD ) this + /*offys.m_flAnimTime*/0x3C );
	}

	__forceinline usercmd& m_PlayerCommand( ) {
		return get< usercmd >( 0x326C );
	}

	__forceinline usercmd*& m_pCurrentCommand( ) {
		return get< usercmd* >( 0x3314 );
	}

	vec3_t WorldSpaceCenter( ) {
		vec3_t Max = this->collideable( )->maxs( ) + this->abs_origin( );
		vec3_t Min = this->collideable( )->mins( ) + this->abs_origin( );
		vec3_t Size = Max - Min;
		Size /= 2;
		Size += Min;
		return Size;
	}

	void SetPlayerAnimState( animstate* state );

	uint32_t& GetMostRecentModelBoneCounter( ) // for fake matrix
	{
		return *reinterpret_cast< uint32_t* >( uintptr_t( this ) + 0x2690 );
	}

	float& GetLastBoneSetupTime( ) // for fake matrix
	{
		return *reinterpret_cast< float* >( uintptr_t( this ) + 0x2924 );
	}

	const char* GetClassname( )
	{
		return CallVFunc<const char*>( this , 142 );
	}

	unsigned int PhysicsSolidMaskForEntity( )
	{
		return CallVFunc<unsigned int>( this , 151 );
	}

	int IsMaxHealth( )
	{
		// @ida: FF 90 ? ? ? ? 85 C0 0F 8F ? ? ? ? 80 + 0x2
		return CallVFunc<int>( this , 122 );
	}

	float old_simtime( )
	{
		static int m_flOldSimulationTime = netvar_manager::netvar_hook_manager.GetOffset( "DT_BaseEntity" , "m_flSimulationTime" ) + 4;
		return *reinterpret_cast< float* >( reinterpret_cast< std::uintptr_t >( this ) + m_flOldSimulationTime );
	}

	void invalidate_physics( std::int32_t flags );
	vec3_t weapon_shoot_pos( );
	vec3_t eye_pos( );
	vec3_t eye_pos_offset( ) {
		return this->origin( ) + this->view_offset( );
	}

	bool& pin_pulled( ) {
		static int m_pin = netvar_manager::netvar_hook_manager.GetOffset( "DT_BaseCSGrenade" , "m_bPinPulled" );
		return *reinterpret_cast< bool* >( ( DWORD ) this + m_pin );
	}

	UINT* m_hMyWeapons( )
	{
		return reinterpret_cast< UINT* >( uintptr_t( this ) + 0x2DF8 );
	}

	void think( ) {
		vfunc< void( __thiscall* )( void* ) >( this , 138 )( this );
	}

	void pre_think( ) {
		vfunc< void( __thiscall* )( void* ) >( this , 317 )( this );
	}

	void post_think( ) {
		vfunc< void( __thiscall* )( void* ) >( this , 318 )( this );
	}

	void* renderable( ) {
		return reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
	}
	void* networkable( ) {
		return reinterpret_cast< void* >( uintptr_t( this ) + 0x8 );
	}

	int	move_type( ) {
		return *reinterpret_cast< int* > ( reinterpret_cast< uintptr_t >( this ) + 0x25C ); //hazedumped
	}

	animstate* get_animstate( );

	bool physics_run_think( int unk01 );

	bool is_breakable_entity( );

	void modify_eye_pos( animstate* anim , vec3_t* pos );

	CBoneAccessor& m_BoneAccessor( );

	vec3_t pred_eyepos( int ticks = 2);

	void inval_bone_cache( );

	void set_abs_angles( const vec3_t& angles );

	void set_abs_origin( const vec3_t& position );

	std::uint32_t& bone_count( );

	matrix_t* bone_cache( );

	vec3_t get_hitbox_pos( int hitbox_id );
	vec3_t get_hitbox_pos( int hitbox_id , matrix_t* matrix );

	float desync_amount( );

	void reset( animstate* state );

	void update_animations( );

	void create_animstate( animstate* state );

	c_weapon* weapon( );

	bool setup_bones( matrix_t* out , int max_bones , int mask , float time );

	animlayer* anim_layers( ) {
		return *reinterpret_cast< animlayer** >( std::uintptr_t( this ) + 0x2980 );
	}

	void force_velocity( const vec3_t& vel ) {
		this->invalidate_physics( 0x4 );
		this->abs_velocity( ) = vel;
	}

	animlayer* anim_layer( int i )
	{
		if ( i < 15 )
			return &anim_layers( )[ i ];
		return nullptr;
	}

	std::array< float , 24 >& poses( )
	{
		return *reinterpret_cast< std::array<float , 24>* >( reinterpret_cast< uintptr_t >( this ) + 0x2774 );
	}

	void* mdl( void ) {
		void* r = this->renderable( );

		if ( !r )
			return nullptr;

		using getmdl_fn = void* ( __thiscall* )( void* );
		return vfunc< getmdl_fn >( r , 8 )( r );
	}

	studiohdr_t* studio_mdl( void* mdl ) {
		using getstudiomdl_fn = studiohdr_t * ( __thiscall* )( void* , void* );
		return vfunc< getstudiomdl_fn >( this , 32 )( this , mdl );
	}
	vec3_t& abs_origin( ) {
		using fn = vec3_t & ( __thiscall* )( void* );
		return vfunc< fn >( this , 10 )( this );
	}

	vec3_t& abs_angles( ) {
		using fn = vec3_t & ( __thiscall* )( void* );
		return vfunc< fn >( this , 11 )( this );
	}

	collideable_t* collideable( ) {
		using original_fn = collideable_t * ( __thiscall* )( void* );
		return ( *( original_fn** ) this )[ 3 ]( this );
	}

	vec3_t& mins( )
	{
		return this->collideable( )->mins( );
	}
	vec3_t& maxs( )
	{
		return this->collideable( )->maxs( );
	}

	client_class_t* client_class( ) {
		using original_fn = client_class_t * ( __thiscall* )( void* );
		return ( *( original_fn** ) networkable( ) )[ 2 ]( networkable( ) );
	}

	int idx( ) {
		using original_fn = int( __thiscall* )( void* );
		return ( *( original_fn** ) networkable( ) )[ 10 ]( networkable( ) );
	}

	bool is_player( )
	{
#if 0
		using original_fn = bool( __thiscall* )( base_entity* );
		return ( *( original_fn** ) this )[ 157 ]( this );
#endif
		return client_class( )->class_id == EClassIds::CCSPlayer;
	}

	void SetAnimLayers( animlayer* layers ) {
		std::memcpy( anim_layers( ) , layers , sizeof( animlayer ) * 13 );
	}

	void GetAnimLayers( animlayer* layers ) {
		std::memcpy( layers , anim_layers( ) , sizeof( animlayer ) * 13 );
	}

	void SetPoseParameters( float* pose ) {
		std::memcpy( m_flPoseParameter( ) , pose , sizeof( float ) * 24 );
	}

	float* m_flPoseParameter( ) {
		return ( float* ) ( ( uintptr_t ) this + 0x2774 );
	}

	void GetPoseParameters( float* poses ) {
		std::memcpy( poses , m_flPoseParameter( ) , sizeof( float ) * 24 );
	}

	bool is_alive( ) {
		return this->health( ) > 0;
	}
	varmapping_t* varmap( ) {
		return ( varmapping_t* ) ( ( uintptr_t ) this + 0x24 );
	}
	void should_interp( bool interp ) {
		auto map = this->varmap( );
		if ( !map ) return;

		for ( auto i = 0; i < map->m_interpolatedentries; i++ ) {
			map->m_entries[ i ].m_needstointerpolate = interp;
		}
	}
	bool is_weapon( ) {
		using original_fn = bool( __thiscall* )( base_entity* );
		return ( *( original_fn** ) this )[ 166 ]( this );
	}

	void get_render_bounds( vec3_t& mins , vec3_t& maxs ) {
		using o_fn = void( __thiscall* )( void* , vec3_t& , vec3_t& );
		return vfunc< o_fn >( renderable( ) , 17 )( renderable( ) , mins , maxs );
	}

	vec3_t& const render_origin( ) {
		using fn = vec3_t& const( __thiscall* )( void* );
		return vfunc< fn >( renderable( ) , 1 )( renderable( ) );
	}

	bool has_c4( )
	{
		static auto fnHasC4 = reinterpret_cast< bool( __thiscall* )( void* ) >( utilities::find_sig( "client.dll" , "56 8B F1 85 F6 74 31" ) );
		return fnHasC4( this );
	}

	uint32_t handle( ) {
		using fn = std::uint32_t( __thiscall* )( void* );
		static auto get_ehandle = ( fn ) ( utilities::find_sig( "client.dll" , "8B 51 3C 83 FA FF" ) );
		return get_ehandle( this );
	}

	vec3_t get_bone_pos( int i )
	{
		matrix_t matrix[ 128 ];
		if ( this->setup_bones( matrix , 128 , 0x00000100 , GetTickCount64( ) ) )
		{
			return vec3_t( matrix[ i ][ 0 ][ 3 ] , matrix[ i ][ 1 ][ 3 ] , matrix[ i ][ 2 ][ 3 ] );
		}
		return vec3_t( 0 , 0 , 0 );
	}
	matrix_t**& b_cache( )
	{
		return get< matrix_t** >( 0x2910 );
	}
	std::string armor_name( )
	{
		if ( armor( ) > 0 )
		{
			if ( has_helmet( ) )
				return "HK";
			else
				return "K";
		}
		else
			return " ";
	}

	bool can_see_player_pos( base_entity* player , const vec3_t& pos );

	int get_sequence( int seq );

	bool enemy( base_entity* from );

	void net_release( ) {
		using original_fn = void( __thiscall* )( void* );
		return ( *( original_fn** ) networkable( ) )[ 1 ]( networkable( ) );
	}

	int net_set_destroyed_on_recreate_entities( ) {
		using original_fn = int( __thiscall* )( void* );
		return ( *( original_fn** ) networkable( ) )[ 13 ]( networkable( ) );
	}

	void net_pre_data_update( int update_type ) {
		using original_fn = void( __thiscall* )( void* , int );
		return ( *( original_fn** ) networkable( ) )[ 6 ]( networkable( ) , update_type );
	}
};

class c_weapon : public base_entity
{
public:
	netvar( float , next_secondary_attack , "DT_BaseCombatWeapon" , "m_flNextSecondaryAttack" );
	netvar( float_t , throw_time , "DT_BaseCSGrenade" , "m_fThrowTime" );
	//netvar( float , get_accuracy_penalty , , "DT_BaseCombatWeapon" , "m_fAccuracyPenalty" );
	netvar( std::uint32_t , get_world_model , "DT_BaseCombatWeapon" , "m_hWeaponWorldModel" );
	netvar( int32_t , world_model_index , "DT_BaseWeaponWorldModel" , "m_nModelIndex" );
	netvar( int , zoom , "DT_WeaponCSBaseGun" , "m_zoomLevel" );
	netvar( float , postpone_fire , "DT_BaseCombatWeapon" , "m_flPostponeFireReadyTime" );
	offset( float , next_primary_attack , 0x3238 );
	offset( int , clip1_count , 0x3264 );
	offset( short , item_definition_index , 0x2FAA );
	offset( float , m_flFallbackWear , 0x31D0 );
	offset( int , m_nFallbackPaintKit , 0x31C8 );
	offset( int , m_nFallbackSeed , 0x31CC );
	offset( int , m_nFallbackStatTrak , 0x31D4 );
	offset( int , m_iEntityQuality , 0x2FAC );
	offset( int , m_iItemIDHigh , 0x2FC0 );
	offset( int , m_iAccountID , 0x2FC8 );
	offset( int , m_OriginalOwnerXuidLow , 0x31C0 );
	offset( float , get_accuracy_penalty , 0x3330 );
	bool is_scopeable( )
	{
		return ( item_definition_index( ) == scar20
			|| item_definition_index( ) == g3sg1
			|| item_definition_index( ) == awp
			|| item_definition_index( ) == ssg08
			|| item_definition_index( ) == aug
			|| item_definition_index( ) == sg553 );
	}

	float& last_shot_time( )
	{
		static int m_fLastShotTime = netvar_manager::netvar_hook_manager.GetOffset( "DT_WeaponCSBase" , "m_fLastShotTime" );
		return *reinterpret_cast< float* >( uintptr_t( this ) + m_fLastShotTime );
	}
	weapon_info_t* weapon_data( );

	bool is_pistol( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { glock, elite, p250, tec9, cz75, usps, p2000, fiveseven };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_deagle( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { deagle };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_awp( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { awp };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_scout( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { ssg08 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}



	bool is_auto( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { g3sg1,scar20 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_sniper( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { awp, ssg08 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_smg( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { mac10, mp7, ump45, p90, bizon, mp9, mp5sd };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_heavy( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { revolver, deagle };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_shotgun( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { nova, xm1014,mag7,sawedoff };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_knife( ) {
		return client_class( )->class_id == ( int ) EClassIds::CKnife;
	}
	bool is_zeus( ) {
		return client_class( )->class_id == ( int ) EClassIds::CWeaponTaser;
	}
	bool is_grenade( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { frag_grenade, hegrenade, inc, smokegrenade, tagrenade, molotov, decoy,flashbang };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}
	bool is_c4( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { c4 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}
	bool is_taser( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { zeus };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_bomb( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { c4 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	bool is_rifle( ) {
		if ( !this )
			return false;

		int id = this->item_definition_index( );
		static const std::vector<int> v = { ak47,aug,famas,galil,m4a1s,m4a4,sg553 };
		return ( std::find( v.begin( ) , v.end( ) , id ) != v.end( ) );
	}

	void update_accuracy( ) {
		using fn = void( __thiscall* )( void* );
		vfunc< fn >( this , 483 )( this );
	}

	std::string weap_name( )
	{
		if ( !this || !item_definition_index( ) )
			return "invalid";

		int weap_id = this->item_definition_index( );
		switch ( weap_id ) {
		case deagle:
			return "Deagle";
			break;
		case elite:
			return "Elites";
			break;
		case fiveseven:
			return "Five Seven";
			break;
		case glock:
			return "Glock";
			break;
		case ak47:
			return "AK-47";
			break;
		case aug:
			return "Aug";
			break;
		case awp:
			return "Awp";
			break;
		case famas:
			return "Famas";
			break;
		case g3sg1:
			return "G3SG1";
			break;
		case galil:
			return "Galil-AR";
			break;
		case m249:
			return "M249";
			break;
		case m4a1s:
			return "M4A1-s";
			break;
		case m4a4:
			return "M4A4";
			break;
		case mac10:
			return "MAC-10";
			break;
		case p90:
			return "P90";
			break;
		case mp5sd:
			return "MP5";
			break;
		case ump45:
			return "UMP-45";
			break;
		case xm1014:
			return "XM1014";
			break;
		case bizon:
			return "Bizon";
			break;
		case mag7:
			return "MAG7";
			break;
		case negev:
			return "Negev";
			break;
		case sawedoff:
			return "Sawed-off";
			break;
		case tec9:
			return "Tec-9";
			break;
		case zeus:
			return "Taser";
			break;
		case p2000:
			return "P2000";
			break;
		case mp7:
			return "MP7";
			break;
		case mp9:
			return "MP9";
			break;
		case nova:
			return "Nova";
			break;
		case p250:
			return "P250";
			break;
		case shield:
			return "Shield";
			break;
		case scar20:
			return "Scar20";
			break;
		case sg553:
			return "SG553";
			break;
		case ssg08:
			return "SSG-08";
			break;
		case flashbang:
			return "Flashbang";
			break;
		case hegrenade:
			return "HE grenade";
			break;
		case smokegrenade:
			return "Smoke grenade";
			break;
		case molotov:
			return "Molotov";
			break;
		case decoy:
			return "Decoy";
			break;
		case inc:
			return "Incendiary";
			break;
		case c4:
			return "C4";
			break;
		case healthshot:
			return "Health Shot";
			break;
		case usps:
			return "USP-s";
			break;
		case cz75:
			return "CZ75-a";
			break;
		case revolver:
			return "Revolver";
			break;
		case knife_bayonet:
			return "Bayonet";
			break;
		case knife_css:
			return "Classic Knife";
			break;
		case knife_flip:
			return "Flip Knife";
			break;
		case knife_gut:
			return "Gut Knife";
			break;
		case knife_karambit:
			return "Karambit";
			break;
		case knife_m9_bayonet:
			return "M9 Bayonet";
			break;
		case knife_tactical:
			return "Huntsman Knife";
			break;
		case knife_falchion:
			return "Falchion Knife";
			break;
		case knife_survival_bowie:
			return "Bowie Knife";
			break;
		case knife_butterfly:
			return "Butterfly Knife";
			break;
		case knife_push:
			return "Shadow Daggers";
			break;
		case knife_cord:
			return "Paracord Knife";
			break;
		case knife_canis:
			return "Survival Knife";
			break;
		case knife_ursus:
			return "Ursus Knife";
			break;
		case knife_gypsy_jackknife:
			return "Navaja Knife";
			break;
		case knife_outdoor:
			return "Nomad Knife";
			break;
		case knife_stiletto:
			return "Stiletto Knife";
			break;
		case knife_widowmaker:
			return "Talon Knife";
			break;
		case knife_skeleton:
			return "Skeleton Knife";
			break;
		default:
			return "Knife";
		}
	}
	bool is_reloading( )
	{
		static auto reloading = *( uint32_t* ) ( utilities::find_sig( ( "client.dll" ) , "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90" ) + 2 );
		return *( bool* ) ( ( uintptr_t ) this + reloading );
	}

	float inaccuracy( );
	float spread( );
	float max_speed( );
};

class player_resource
{
public:
	const char* get_clantag( int index )
	{
		return ( const char* ) ( ( uintptr_t ) this + netvar_manager::netvar_hook_manager.GetOffset( "DT_CSPlayerResource" , "m_szClan" ) + ( index * 0x10 ) );
	}
};

class CBreakableSurface : public base_entity , public IBreakableWithPropData
{
public:
	netvar( bool , IsBroken , "DT_BreakableSurface" , "m_bIsBroken" );
};