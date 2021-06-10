#include "base_entity.h"
#include "../math/math.h"
#include <interfaces/interfaces.h>
#include <hook.h>
#include "../../vars.h"

animstate* base_entity::get_animstate( ) {
	return *( animstate** ) ( ( DWORD ) this + 0x3914 );
}

bool base_entity::physics_run_think( int unk01 ) {
	using original_fn = bool( __thiscall* )( void* , int );

	static auto func = original_fn( utilities::find_sig( "client.dll" , "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87 ? ? ? ? C1 E8 16" ) );
	return func( this , unk01 );
}
void animstate_pose_param_cache::set_value( base_entity* e , float val ) {
	if ( m_idx >= 0 )
		e->poses( )[ m_idx ] = val;
}

bool base_entity::is_breakable_entity( )
{
	using original_fn = bool( __thiscall* )( void* );
	static auto func = original_fn( utilities::find_sig( "client.dll" , "55 8B EC 51 56 8B F1 85 F6 74 68" ) );
	return func( this );
}

void base_entity::modify_eye_pos( animstate* anim , vec3_t* pos ) {
	using bone_lookup_fn = int( __thiscall* )( void* , const char* );
	static auto lookup_bone = ( bone_lookup_fn ) utilities::find_sig( "client.dll" , "55 8B EC 53 56 8B F1 57 83 BE ? ? ? ? ? 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 ? ? ? ? 8B 8E ? ? ? ? 85 C9 0F 84" );

	if ( anim->hit_ground && anim->duck_amount != 0.0f ) {
		auto bone_pos = this->bone_cache( )[ lookup_bone( this , "head_0" ) ].at( 3 );

		bone_pos.z += 1.7f;

		if ( ( *pos ).z > bone_pos.z ) {
			auto some_factor = 0.0f;
			auto delta = ( *pos ).z - bone_pos.z;
			auto some_offset = ( delta - 4.0f ) / 6.0f;

			if ( some_offset >= 0.0f )
				some_factor = std::fminf( some_offset , 1.0f );

			( *pos ).z += ( ( bone_pos.z - ( *pos ).z ) * ( ( ( some_factor * some_factor ) * 3.0f ) - ( ( ( some_factor * some_factor ) * 2.0f ) * some_factor ) ) );
		}
	}
}
CBoneAccessor& base_entity::m_BoneAccessor( )
{
	static auto offset = *( std::uintptr_t* ) ( ( std::uint32_t )( utilities::find_sig( "client.dll" , "8D 81 ? ? ? ? 50 8D 84 24" ) + 2 ) );
	return get<CBoneAccessor>( offset );
}

/*

BoneAccessor = pattern::find(m_client_dll, XOR("8D 81 ? ? ? ? 50 8D 84 24")).add(2).to< size_t >();
*/

void base_entity::invalidate_physics( std::int32_t flags ) {
	static const auto invalidate_physics_recursive = reinterpret_cast< void( __thiscall* )( void* , int32_t ) >(
		utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56" ) );
	invalidate_physics_recursive( this , flags );
}

vec3_t base_entity::weapon_shoot_pos( )
{
	/* only on local ( when fakeducking ) */
	if ( /*g_var.ragebot.antiaim.main.duck_key.is_active( ) &&*/ this->idx( ) == interfaces::engine->get_local_player( ) ) {
		vec3_t m_origin = this->origin( );

		vec3_t m_duck_hull_min = interfaces::game_movement->GetPlayerMins( true );
		vec3_t m_stand_hull_min = interfaces::game_movement->GetPlayerMins( false );

		float m_more = ( m_duck_hull_min.z - m_stand_hull_min.z );

		vec3_t m_duck_view_offset = interfaces::game_movement->GetPlayerViewOffset( true );
		vec3_t m_stand_view_offset = interfaces::game_movement->GetPlayerViewOffset( false );
		float m_duck_fraction = this->duck_amount( );

		float m_tempz = ( ( m_duck_view_offset.z - m_more ) * m_duck_fraction ) +
			( m_stand_view_offset.z * ( 1 - m_duck_fraction ) );

		m_origin.z += m_tempz;

		return m_origin;
	}

	auto pos = this->eye_pos_offset( );

	if ( OFFSET( std::uint32_t , 0x3AB4 ) ) {
		auto animstate = this->get_animstate( );
		if ( animstate ) {
			modify_eye_pos( animstate , &pos );
		}
	}

	return pos;
}

vec3_t base_entity::eye_pos( ) {

	return this->weapon_shoot_pos();
}

void simulate_movement( base_entity* e , vec3_t& pred , int choked )
{
	pred = vec3_t( e->eye_pos( ).x + ( e->velocity( ).x * interfaces::global_vars->interval_per_tick ) * choked , e->eye_pos( ).y + ( e->velocity( ).y * interfaces::global_vars->interval_per_tick ) * choked , e->eye_pos( ).z );
}
vec3_t base_entity::pred_eyepos( int ticks )
{
	vec3_t pred;
	simulate_movement( this , pred , ticks );
	return pred;
}
void base_entity::inval_bone_cache( ) {
	static auto invalidate_bone_cache = std::uintptr_t( utilities::find_sig( "client.dll" , "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) + 10 );

	*( std::uint32_t* ) ( ( std::uintptr_t ) this + 0x2924 ) = 0xFF7FFFFF;
	*( std::uint32_t* ) ( ( std::uintptr_t ) this + 0x2690 ) = **( std::uintptr_t** ) invalidate_bone_cache - 1;
}

void base_entity::set_abs_angles( const vec3_t& angles ) {
	using original_fn = void( __thiscall* )( void* , const vec3_t& );
	static original_fn set_angles_fn = ( original_fn ) ( ( DWORD ) utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1" ) );
	set_angles_fn( this , angles );
}

void base_entity::set_abs_origin( const vec3_t& position ) {
	using original_fn = void( __thiscall* )( void* , const vec3_t& );
	static original_fn set_position_fn = ( original_fn ) ( ( DWORD ) utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D" ) );
	set_position_fn( this , position );
}

std::uint32_t& base_entity::bone_count( ) {
	static auto offset = *( std::uintptr_t* )( ( std::uint32_t )( utilities::find_sig( "client.dll" , "8B 87 ? ? ? ? 8B 4D 0C" ) + 2 ) );
	return *( uint32_t* ) ( std::uintptr_t( renderable( ) ) + offset );
}

matrix_t* base_entity::bone_cache( ) {
	static auto offset = *( std::uintptr_t* ) ( ( std::uint32_t )( utilities::find_sig( "client.dll" , "FF B7 ? ? ? ? 52" ) + 2 ) );
	return *( matrix_t** ) ( std::uintptr_t( renderable( ) ) + offset );
}

vec3_t base_entity::get_hitbox_pos( int hitbox_id )
{
	auto studio_model = interfaces::model_info->studio_model( this->mdl( ) );
	if ( studio_model ) {
		auto hitbox = studio_model->hitbox_set( 0 )->hitbox( hitbox_id );
		if ( hitbox ) {
			auto
				min = vec3_t{} ,
				max = vec3_t{};

			math::VectorTransform( hitbox->bbmin , this->bone_accessor( ).get_bone( hitbox->bone ) , min );
			math::VectorTransform( hitbox->bbmax , this->bone_accessor( ).get_bone( hitbox->bone ) , max );

			return ( min + max ) / 2.0f;
		}
	}
	return vec3_t{};
}
vec3_t base_entity::get_hitbox_pos( int hitbox_id , matrix_t* matrix )
{
	auto studio_model = interfaces::model_info->studio_model( this->mdl( ) );

	if ( studio_model ) {
		auto hitbox = studio_model->hitbox_set( 0 )->hitbox( hitbox_id );

		if ( hitbox ) {
			vec3_t min{} , max{};

			math::VectorTransform( hitbox->bbmin , matrix[ hitbox->bone ] , min );
			math::VectorTransform( hitbox->bbmax , matrix[ hitbox->bone ] , max );

			return ( min + max ) / 2.0f;
		}
	}
	return vec3_t{};
}
float base_entity::desync_amount( ) {
	auto state = this->get_animstate( );

	if ( !state )
		return 0.0f;

	float running_speed = std::clamp( state->run_speed , 0.0f , 1.0f );
	float yaw_modifier = ( ( ( state->ground_fraction * -0.3f ) - 0.2f ) * running_speed ) + 1.0f;
	if ( state->duck_amount > 0.0f )
	{
		float ducking_speed = std::clamp( state->duck_speed , 0.0f , 1.0f );
		yaw_modifier += ( ( state->duck_amount * ducking_speed ) * ( 0.5f - yaw_modifier ) );
	}
	return yaw_modifier * state->max_yaw;
}
void base_entity::reset( animstate* state ) {

	using ResetAnimState_t = void( __thiscall* )( animstate* );
	static auto func = ( ResetAnimState_t ) utilities::find_sig( "client.dll" , "56 6A 01 68 ? ? ? ? 8B F1" );
	func( state );
}

void animstate::update( const vec3_t& ang ) {
	using update_fn = void( __vectorcall* )( void* , void* , float , float , float , void* );
	static auto update_animstate = reinterpret_cast< update_fn >( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54" ) );
	update_animstate( this , nullptr , ang.z , ang.y , ang.x , nullptr );
}
void base_entity::update_animations( ) {
	using fn = void( __thiscall* )( void* );
	return vfunc< fn >( this , 223 )( this );
}

void base_entity::create_animstate( animstate* state ) {
	using create_fn = void( __thiscall* )( void* , void* );
	static auto create_animstate = reinterpret_cast< create_fn >( utilities::find_sig( "client.dll" , "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" ) );
	create_animstate( state , this );
}

c_weapon* base_entity::weapon( ) {
	return ( c_weapon* ) interfaces::ent_list->get_by_handle( weapon_handle( ) );
}

bool base_entity::setup_bones( matrix_t* out , int max_bones , int mask , float time ) {
	if ( !this ) {
		return false;
	}
	//return hook::setup_bones_fn( this->renderable( ) , nullptr , out , max_bones , mask , time );
	using original_fn = bool( __thiscall* )( void* , matrix_t* , int , int , float );
	return ( *( original_fn** ) renderable( ) )[ 13 ]( renderable( ) , out , max_bones , mask , time );
}

weapon_info_t* c_weapon::weapon_data( ) {
	using fn = weapon_info_t * ( __thiscall* )( void* );
	return vfunc< fn >( this , 460 )( this );
}

float c_weapon::inaccuracy( ) {
	using fn = float( __thiscall* )( void* );
	return vfunc< fn >( this , 482 )( this );
}

float c_weapon::spread( ) {
	using fn = float( __thiscall* )( void* );
	return vfunc< fn >( this , 452 )( this );
}

float c_weapon::max_speed( ) {
	using fn = float( __thiscall* )( void* );
	return vfunc< fn >( this , 439 )( this );
}

bool base_entity::can_see_player_pos( base_entity* player , const vec3_t& pos ) {
	trace_t tr;
	ray_t ray;
	trace_filter filter;
	filter.skip = this;

	auto start = eye_pos( );
	auto dir = ( pos - start ).normalized( );

	ray.initialize( start , pos );
	interfaces::trace_ray->trace_ray( ray , mask_shot | contents_grate , &filter , &tr );

	return tr.entity == player || tr.flFraction > 0.97f;
}

int base_entity::get_sequence( int seq )
{
	studiohdr_t* studio = interfaces::model_info->studio_model( this->mdl( ) );
	if ( studio == nullptr )
		return -1;

	using seq_fn = int( __fastcall* )( void* , void* , int );
	static auto oGetSequenceActivity = reinterpret_cast< seq_fn >( utilities::find_sig( "client.dll" , "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );
	return oGetSequenceActivity( this , studio , seq );
}

bool base_entity::enemy( base_entity* from )
{
	if ( !this || !from )
		return false;

	if ( this == from )
		return false;

	if ( team( ) != from->team( ) )
		return true;
	else if ( interfaces::cvar->find_var( "mp_teammates_are_enemies" )->get_int( ) )
		return true;

	return false;
}

bool& base_entity::client_animations( )
{
	static int offset = netvar_manager::netvar_hook_manager.GetOffset( "DT_BaseAnimating" , "m_bClientSideAnimation" );
	return *reinterpret_cast< bool* >( uintptr_t( this ) + offset );
}

void base_entity::SetPlayerAnimState( animstate* state ) {
	set< animstate* >( interfaces::PlayerAnimState , state );
}
