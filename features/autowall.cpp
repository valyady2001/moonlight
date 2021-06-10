#include "features.h"

#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD        1
#define HITGROUP_CHEST        2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_NECK   8
#define HITGROUP_GEAR        10

#define	DAMAGE_NO				0
#define DAMAGE_EVENTS_ONLY		1		// Call damage functions, but don't modify health
#define	DAMAGE_YES				2
#define	DAMAGE_AIM				3


float DotProduct( const vec3_t& a , const vec3_t& b ) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DistanceToRay( const vec3_t& pos , const vec3_t& rayStart , const vec3_t& rayEnd , float* along = NULL , vec3_t* pointOnRay = NULL )
{
	vec3_t to = pos - rayStart;
	vec3_t dir = rayEnd - rayStart;
	float length = dir.normalize_in_place( );

	float rangeAlong = DotProduct( dir , to );
	if ( along )
	{
		*along = rangeAlong;
	}

	float range;

	if ( rangeAlong < 0.0f )
	{
		// off start point
		range = -( pos - rayStart ).length( );

		if ( pointOnRay )
		{
			*pointOnRay = rayStart;
		}
	}
	else if ( rangeAlong > length )
	{
		// off end point
		range = -( pos - rayEnd ).length( );

		if ( pointOnRay )
		{
			*pointOnRay = rayEnd;
		}
	}
	else // within ray bounds
	{
		vec3_t onRay = rayStart + rangeAlong * dir;
		range = ( pos - onRay ).length( );

		if ( pointOnRay )
		{
			*pointOnRay = onRay;
		}
	}

	return range;
}

void util_clip_trace_to_players( const vec3_t& start , const vec3_t& end , unsigned int mask , trace_filter* filter , trace_t* trace , float range = 0 , float fraction = -1 )
{
	trace_t player_trace;
	ray_t ray;
	if ( fraction < 0.0f )
		fraction = trace->flFraction;
	const float maxRange = 60.0f;

	ray.initialize( start , end );

	for ( int i = 0; i <= 64; i++ )
	{
		base_entity* player = ( base_entity* ) interfaces::ent_list->get_client_entity( i );

		if ( !player || !player->is_alive( ) )
			continue;

		if ( player->dormant( ) )
			continue;

		if ( filter && filter->ShouldHitEntity( player , mask ) == false )
			continue;

		float m_range = DistanceToRay( player->WorldSpaceCenter( ) , start , end );

		if ( m_range < range || m_range > maxRange )
			continue;

		interfaces::trace_ray->clip_ray_to_entity( ray , mask | contents_hitbox , player , &player_trace );
		if ( player_trace.flFraction < fraction )
		{
			// we shortened the ray - save off the trace
			*trace = player_trace;
			fraction = player_trace.flFraction;
		}
	}
}
namespace features
{
	namespace autowall
	{
		float scale( base_entity* player , float& damage , float armor_ratio , int hitgroup )
		{
			bool  has_heavy_armor;
			int   armor;
			float heavy_ratio , bonus_ratio , ratio , new_damage;

			auto is_armored = [ ] ( base_entity* player , int armor , int hitgroup ) {
				// the player has no armor.
				if ( armor <= 0 )
					return false;

				// if the hitgroup is head and the player has a helment, return true.
				// otherwise only return true if the hitgroup is not generic / legs / gear.
				if ( hitgroup == hitgroup_head && player->has_helmet( ) )
					return true;

				else if ( hitgroup >= hitgroup_chest && hitgroup <= hitgroup_rightarm )
					return true;

				return false;
			};

			// check if the player has heavy armor, this is only really used in operation stuff.
			has_heavy_armor = false; // player->has_heavy_armor( );

			// scale damage based on hitgroup.
			switch ( hitgroup ) {
			case hitgroup_head:
				if ( has_heavy_armor )
					damage = ( damage * 4.f ) * 0.5f;
				else
					damage *= 4.f;
				break;

			case hitgroup_stomach:
				damage *= 1.25f;
				break;

			case hitgroup_leftleg:
			case hitgroup_rightleg:
				damage *= 0.75f;
				break;

			default:
				break;
			}

			// grab amount of player armor.
			armor = player->armor( );

			// check if the ent is armored and scale damage based on armor.
			if ( is_armored( player , armor , hitgroup ) ) {
				heavy_ratio = 1.f;
				bonus_ratio = 0.5f;
				ratio = armor_ratio * 0.5f;

				// player has heavy armor.
				if ( has_heavy_armor ) {
					// calculate ratio values.
					bonus_ratio = 0.33f;
					ratio = armor_ratio * 0.25f;
					heavy_ratio = 0.33f;

					// calculate new damage.
					new_damage = ( damage * ratio ) * 0.85f;
				}

				// no heavy armor, do normal damage calculation.
				else
					new_damage = damage * ratio;

				if ( ( ( damage - new_damage ) * ( heavy_ratio * bonus_ratio ) ) > armor )
					new_damage = damage - ( armor / bonus_ratio );

				damage = new_damage;
			}

			return damage;
		}
		bool breakable( base_entity* e )
		{
			static auto IsBreakableEntity = ( bool( __thiscall* )( base_entity* ) )( utilities::find_sig( "client.dll" , "55 8B EC 51 56 8B F1 85 F6 74 68" ) );

			bool        ret;
			client_class_t* cc;
			const char* name;
			char* takedmg , old_takedmg;

			static size_t m_takedamage_offset{ *( size_t* ) ( ( uintptr_t ) IsBreakableEntity + 0x26 ) };

			// skip null ents and the world ent.
			if ( !e || e->idx( ) == 0 )
				return false;

			// get m_takedamage and save old m_takedamage.
			takedmg = ( char* ) ( ( uintptr_t ) e + m_takedamage_offset );
			old_takedmg = *takedmg;

			// get clientclass.
			cc = e->client_class( );

			if ( cc ) {
				// get clientclass network name.
				name = cc->network_name;

				// CBreakableSurface, CBaseDoor, ...
				if ( name[ 1 ] != 'F'
					|| name[ 4 ] != 'c'
					|| name[ 5 ] != 'B'
					|| name[ 9 ] != 'h' ) {
					*takedmg = DAMAGE_YES;
				}
			}

			ret = IsBreakableEntity( e );
			*takedmg = old_takedmg;
			return ret;

#if 0
			if ( !e || !e->idx( ) )
				return false;
			int iHealth = e->health( );
			if ( e->GetTakeDamage( ) != DAMAGE_YES )
				return false;
			int CollisionGroup = e->GetCollisionGroup( );
			if ( CollisionGroup != COLLISION_GROUP_PUSHAWAY && CollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && CollisionGroup != COLLISION_GROUP_NONE )
				return false;
			if ( iHealth > 200 )
				return false;
			IMultiplayerPhysics* pPhysicsInterface = ( IMultiplayerPhysics* ) e;
			if ( pPhysicsInterface ) {
				if ( pPhysicsInterface->GetMultiplayerPhysicsMode( ) != PHYSICS_MULTIPLAYER_SOLID )
					return false;
			}
			else
			{
				const char* szClassname = e->GetClassname( );
				if ( szClassname [ 0 ] == 'f' && szClassname [ 5 ] == 'b' && szClassname [ 13 ] == 'e' )
				{
					if ( szClassname [ 15 ] == 's' )
					{
						// don't try to break it if it has already been broken
						if ( e->is_broken( ) )
							return false;
					}
				}
				else if ( e->PhysicsSolidMaskForEntity( ) & contents_playerclip )
				{
					// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
					return false;
				}
			}
			IBreakableWithPropData* pBreakableInterface = ( IBreakableWithPropData* ) e;
			if ( pBreakableInterface )
				if ( pBreakableInterface->GetDmgModBullet( ) <= 0.0f )
					return false;

			return true;
#endif
		}
		bool trace_to_exit( const vec3_t& start , const vec3_t& dir , vec3_t& out , trace_t* enter_trace , trace_t* exit_trace )
		{
			static trace_filter_simple_game_t filter {};

			float dist = 0.0f;
			vec3_t new_end;
			int contents , first_contents = 0;
			ray_t ray;

			// max pen distance is 90 units.
			while ( dist <= 90.f ) {
				// step forward a bit.
				dist += 4.f;

				// set out pos.
				out = start + ( dir * dist );

				contents = interfaces::trace_ray->get_point_contents( out , mask_shot , nullptr );

				if ( !first_contents )
					first_contents = contents;

				if ( !( contents & mask_shot_hull ) || contents & contents_hitbox && contents != first_contents ) {

					// move end pos a bit for tracing.
					new_end = out - ( dir * 4.f );

					// do first trace.
					ray.initialize( out , new_end );
					interfaces::trace_ray->trace_ray( ray , mask_shot , nullptr , exit_trace );

					static auto sv_clip_penetration_traces_to_players = interfaces::cvar->find_var( "sv_clip_penetration_traces_to_players" );

					// note; this is some new stuff added sometime around late 2017 ( 10.31.2017 update? ).
					if ( sv_clip_penetration_traces_to_players->get_int( ) ) {
						util_clip_trace_to_players( out , new_end , mask_shot , nullptr , exit_trace , -60.f );
					}

					// we hit an ent's hitbox, do another trace.
					if ( exit_trace->startSolid && ( exit_trace->surface.flags & surf_hitbox ) != 0 ) {
						ray.initialize( out , start );
						filter.set_pass_entity( exit_trace->entity );
						interfaces::trace_ray->trace_ray( ray , mask_shot_hull , ( trace_filter* ) &filter , exit_trace );

						if ( exit_trace->did_hit( ) && !exit_trace->startSolid ) {
							out = exit_trace->end;
							return true;
						}

					}
					else {
						if ( !exit_trace->did_hit( ) || exit_trace->startSolid ) {
							if ( enter_trace->DidHitNonWorldEntity( ) && ( breakable( enter_trace->entity ) || enter_trace->entity->client_class( )->class_id == EClassIds::CBaseEntity ) ) {
								*exit_trace = *enter_trace;
								exit_trace->end = start + dir;
								return true;
							}

						}
						else {
							if ( ( exit_trace->surface.flags & surf_nodraw ) ) {
								// note; server doesnt seem to check CGameTrace::DidHitNonWorldEntity for both of these breakable checks?
								if ( breakable( exit_trace->entity ) && breakable( enter_trace->entity ) ) {
									out = exit_trace->end;
									return true;
								}

								if ( !( enter_trace->surface.flags & surf_nodraw ) )
									continue;
							}

							if ( exit_trace->plane.normal.dot( dir ) <= 1.f ) {
								out -= ( dir * ( exit_trace->flFraction * 4.f ) );
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		void clip_trace_to_player( const vec3_t& start , const vec3_t& end , uint32_t mask , trace_t* tr , base_entity* player , float min )
		{
#if 0
			vec3_t pos , to , dir , on_ray;
			float len , range_along , range;
			ray_t ray;
			trace_t new_trace;

			// reference: https://github.com/alliedmodders/hl2sdk/blob/3957adff10fe20d38a62fa8c018340bf2618742b/game/shared/util_shared.h#L381

			// set some local vars.
			pos = player->origin( ) + ( ( player->mins( ) + player->maxs( ) ) * 0.5f );
			to = pos - start;
			dir = start - end;
			len = dir.fnormalize_place( );
			range_along = dir.dot( to );

			// off start point.
			if ( range_along < 0.f )
				range = -( to ).length( );

			// off end point.
			else if ( range_along > len )
				range = -( pos - end ).length( );

			// within ray bounds.
			else {
				on_ray = start + ( dir * range_along );
				range = ( pos - on_ray ).length( );
			}

			if ( /*min <= range &&*/ range <= 60.f ) {
				// clip to player.
				ray_t m_clip_trace; m_clip_trace.initialize( start , end );
				interfaces::trace_ray->clip_ray_to_entity( m_clip_trace , mask , player , &new_trace );

				if ( tr->flFraction > new_trace.flFraction )
					*tr = new_trace;
			}
#endif

			float range = DistanceToRay( player->WorldSpaceCenter( ) , start , end );
			if ( range < min || range > 60.0f )
				return;

			ray_t ray;
			ray.initialize( start , end );

			trace_t player_trace;
			interfaces::trace_ray->clip_ray_to_entity( ray , mask | contents_hitbox , player , &player_trace );

			if ( player_trace.entity && player_trace.entity == player && tr->flFraction > player_trace.flFraction )
				*tr = player_trace;
		}

		bool is_valid_hitgroup( int idx )
		{
			if ( ( idx >= hitgroup_head && idx <= hitgroup_rightleg ) || idx == hitgroup_gear )
				return true;

			return false;
		}

		bool handle_bullet_penetration(
			base_entity* player_from ,
			float& penetration ,
			int& enter_material ,
			trace_t& enter_trace ,
			vec3_t& dir ,
			float penetration_modifier ,
			float damage_modifier ,
			float penetration_power ,
			int& penetration_count ,
			vec3_t& start ,
			float distance ,
			float current_distance ,
			float& current_damage )
		{
			bool no_draw = !!( enter_trace.surface.flags & ( surf_nodraw ) );
			bool hit_grate = ( enter_trace.contents & contents_grate ) != 0;

			bool failed_penetrate = false;

			// check if bullet can penetrarte another entity
			if ( penetration_count == 0 && !hit_grate && !no_draw
				&& enter_material != char_tex_glass && enter_material != char_tex_grate )
				failed_penetrate = true; // no, stop

			// If we hit a grate with iPenetration == 0, stop on the next thing we hit
			if ( penetration <= 0 || penetration_count <= 0 )
				failed_penetrate = true;

			vec3_t pen_end;

			// find exact penetration exit
			trace_t exit_trace;
			if ( !trace_to_exit( enter_trace.end , dir , pen_end , &enter_trace , &exit_trace ) )
			{
				// ended in solid
				if ( ( interfaces::trace_ray->get_point_contents( enter_trace.end , mask_shot_hull ) & mask_shot_hull ) == 0 )
				{
					failed_penetrate = true;
				}
			}

			if ( failed_penetrate == true )
			{
				return true;
			}

			// get material at exit point
			surfacedata_t* exit_surface_data = interfaces::physics_props->get_surface_data( exit_trace.surface.surfaceProps );
			int exit_material = exit_surface_data->game.material;

			static auto sv_penetration_type = interfaces::cvar->find_var( "sv_penetration_type" );
			static auto ff_damage_reduction_bullets = interfaces::cvar->find_var( "ff_damage_reduction_bullets" );
			static auto ff_damage_bullet_penetration = interfaces::cvar->find_var( "ff_damage_bullet_penetration" );

			// new penetration method
			if ( sv_penetration_type->get_int( ) == 1 )
			{
				// percent of total damage lost automatically on impacting a surface
				float damage_lost_percent = 0.16f;

				// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
				// CONTENTS_GRATE and use a high damage modifier.
				if ( hit_grate || no_draw || enter_material == char_tex_glass || enter_material == char_tex_grate )
				{
					// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
					if ( enter_material == char_tex_glass || enter_material == char_tex_grate )
					{
						penetration_modifier = 3.0f;
						damage_lost_percent = 0.05;
					}
					else
						penetration_modifier = 1.0f;

					damage_modifier = 0.99f;
				}
				else if ( enter_material == char_tex_flesh && ff_damage_reduction_bullets->get_float( ) == 0.f
					&& enter_trace.entity && enter_trace.entity->is_player( ) && enter_trace.entity->team( ) == player_from->team( ) )
				{
					if ( ff_damage_bullet_penetration->get_float( ) == 0.f )
					{
						// don't allow penetrating players when FF is off
						penetration_modifier = 0.f;
						return true;
					}

					penetration_modifier = ff_damage_bullet_penetration->get_float( );
					damage_modifier = ff_damage_bullet_penetration->get_float( );
				}
				else
				{
					// check the exit material and average the exit and entrace values
					float exit_penetration_modifier = exit_surface_data->game.flPenetrationModifier;
					float exit_damage_modifier = exit_surface_data->game.flDamageModifier;
					penetration_modifier = ( penetration_modifier + exit_penetration_modifier ) / 2.f;
					damage_modifier = ( damage_modifier + exit_damage_modifier ) / 2.f;
				}

				// if enter & exit point is wood we assume this is 
				// a hollow crate and give a penetration bonus
				if ( enter_material == exit_material )
				{
					if ( exit_material == char_tex_wood || exit_material == char_tex_cardboard )
					{
						penetration_modifier = 3.f;
					}
					else if ( exit_material == char_tex_plastic )
					{
						penetration_modifier = 2.f;
					}
				}

				float trace_distance = ( exit_trace.end - enter_trace.end ).length( );

				float pen_mod = std::fmaxf( 0.f , ( 1.f / penetration_modifier ) );

				float percent_damage_chunk = current_damage * damage_lost_percent;
				float pen_wep_mod = percent_damage_chunk + std::fmaxf( 0.f , ( 3.f / penetration_power ) * 1.25f ) * ( pen_mod * 3.0f );

				float lost_damage_object = ( ( pen_mod * ( trace_distance * trace_distance ) ) / 24.f );
				float total_lost_damage = pen_wep_mod + lost_damage_object;

				// reduce damage power each time we hit something other than a grate
				current_damage -= std::fmaxf( 0.f , total_lost_damage );

				if ( current_damage < 1.f )
					return true;

				// penetration was successful

				current_distance += trace_distance;

				start = exit_trace.end;
				distance = ( distance - current_distance ) * 0.5f;

				penetration_count--;
				return false;
			}
			else
			{
				// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
				// CONTENTS_GRATE and use a high damage modifier.
				if ( hit_grate || no_draw )
				{
					// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
					penetration_modifier = 1.0f;
					damage_modifier = 0.99f;
				}
				else
				{
					// Check the exit material to see if it is has less penetration than the entrance material.
					float exit_penetration_modifier = exit_surface_data->game.flPenetrationModifier;
					float exit_damage_modifier = exit_surface_data->game.flDamageModifier;
					if ( exit_penetration_modifier < penetration_modifier )
					{
						penetration_modifier = exit_penetration_modifier;
					}
					if ( exit_damage_modifier < damage_modifier )
					{
						damage_modifier = exit_damage_modifier;
					}
				}

				// if enter & exit point is wood or metal we assume this is 
				// a hollow crate or barrel and give a penetration bonus
				if ( enter_material == exit_material )
				{
					if ( exit_material == char_tex_wood ||
						exit_material == char_tex_metal )
					{
						penetration_modifier *= 2.f;
					}
				}

				float trace_distance = ( exit_trace.end - enter_trace.end ).length( );

				// check if bullet has enough power to penetrate this distance for this material
				if ( trace_distance > ( penetration_power * penetration_modifier ) )
					return true; // bullet hasn't enough power to penetrate this distance

				// reduce damage power each time we hit something other than a grate
				current_damage *= damage_modifier;

				// penetration was successful

				penetration_power -= trace_distance / penetration_modifier;
				current_distance += trace_distance;

				start = exit_trace.end;
				distance = ( distance - current_distance ) * 0.5f;

				// reduce penetration counter
				penetration_count--;
				return false;
			}
		}

		bool run( penetration_input_t* in , penetration_output_t* out )
		{
			static trace_filter_skip_two_entities_game_t filter {};

			vec3_t start;
			c_weapon* weapon;
			weapon_info_t* weapon_info;
			base_entity* last_hit_entity = nullptr;

			// if we are tracing from our local player perspective.
			if ( in->m_from == g::local ) {
				weapon = g::local->weapon( );
				weapon_info = g::local->weapon( )->weapon_data( );
				start = g::eye_pos;
			}

			// not local player.
			else {
				weapon = in->m_from->weapon( );
				if ( !weapon )
					return false;

				// get weapon info.
				weapon_info = weapon->weapon_data( );
				if ( !weapon_info )
					return false;

				// set trace start.
				start = in->m_from->weapon_shoot_pos( );
			}

			// get direction to end point.
			//vec3_t dir = math::angle_1vec( math::calculate_angle( start , in->m_pos ) );
			vec3_t dir = ( in->m_pos - start ).normalized( );

			float penetration = weapon_info->penetration;
			float current_damage = ( float ) weapon_info->dmg; // damage of the bullet at it's current trajectory
			float current_distance = 0.0f; // distance that the bullet has traveled so far
			float distance = weapon_info->range;
			float penetration_power = 35.f; // thickness of a wall that this bullet can penetrate
			float penetration_distance = 3000.f; // distance at which the bullet is capable of penetrating a wall
			float damage_modifier = 0.5f; // default modification of bullets power after they go through a wall.
			float penetration_modifier = 1.0f;

			// we use the max penetrations on this gun to figure out how much penetration it's capable of
			static auto sv_penetration_type = interfaces::cvar->find_var( "sv_penetration_type" );
			if ( sv_penetration_type->get_int( ) == 1 )
				penetration_power = penetration;

			// setup trace filter for later.
			filter.set_pass_entity( in->m_from );
			filter.set_pass_entity2( nullptr );

			int penetration_count = 4;
			while ( current_damage > 0.f ) {
				vec3_t end = start + ( dir * ( distance - current_distance ) );

				ray_t ray;
				trace_t enter_trace; // main enter bullet trace

				// setup ray and trace.
				ray.initialize( start , end );
				filter.set_pass_entity( in->m_from );
				filter.set_pass_entity2( last_hit_entity ? last_hit_entity : nullptr );
				interfaces::trace_ray->trace_ray( ray , mask_shot_hull | contents_hitbox , ( trace_filter* ) &filter , &enter_trace );

				// get last hit entity for trace.
				last_hit_entity = enter_trace.entity ? enter_trace.entity : nullptr;

				// check for player hitboxes extending outside their collision bounds.
				// if no target is passed we clip the trace to a specific player, otherwise we clip the trace to any player.
				if ( in->m_target )
					clip_trace_to_player( start , end + ( dir * 40.f ) , mask_shot_hull | contents_hitbox , &enter_trace , in->m_target , -60.f );
				else
					util_clip_trace_to_players( start , end + ( dir * 40.f ) , mask_shot_hull | contents_hitbox , ( trace_filter* ) &filter , &enter_trace , -60.f );

				// we didn't hit anything, stop tracing shoot.
				if ( enter_trace.flFraction == 1.f )
					break;

				// calculate the damage based on the distance the bullet travelled.
				current_distance += enter_trace.flFraction * ( distance - current_distance );
				current_damage *= std::pow( weapon_info->range_modifier , current_distance / 500.f );

				surfacedata_t* enter_surface = interfaces::physics_props->get_surface_data( enter_trace.surface.surfaceProps );

				int enter_material = enter_surface->game.material;

				penetration_modifier = enter_surface->game.flPenetrationModifier;

				damage_modifier = enter_surface->game.flDamageModifier;

				// check if we reach penetration distance, no more penetrations after that
				// or if our modifier is super low, just stop the bullet.
				/*if ( ( current_distance > penetration_distance && penetration > 0.f ) ||
					penetration_modifier < 0.1f )
				{
					penetration_count = 0;
				}*/

				// if a target was passed.
				if ( in->m_target ) {

					// validate that we hit the target we aimed for.
					if ( enter_trace.entity && enter_trace.entity == in->m_target && is_valid_hitgroup( enter_trace.hitGroup ) ) {
						int group = ( weapon->item_definition_index( ) == zeus ) ? hitgroup_generic : enter_trace.hitGroup;

						// scale damage based on the hitgroup we hit.
						float player_damage = scale( in->m_target , current_damage , weapon_info->armor_ratio , group );

						// set result data for when we hit a player.
						out->m_pen = penetration_count != 4;
						out->m_hitgroup = group;
						out->m_damage = player_damage;
						out->m_target = in->m_target;

						// non-penetrate damage.
						if ( penetration_count == 4 )
							return player_damage >= in->m_damage;

						// penetration damage.
						return player_damage >= in->m_damage_pen;
					}
				}

				// no target was passed, check for any player hit or just get final damage done.
				else {
					out->m_pen = penetration_count != 4;

					// todo - dex; team checks / other checks / etc.
					if ( enter_trace.entity && enter_trace.entity->is_player( ) && is_valid_hitgroup( enter_trace.hitGroup ) ) {
						int group = ( weapon->item_definition_index( ) == zeus ) ? hitgroup_generic : enter_trace.hitGroup;

						float player_damage = scale( enter_trace.entity , current_damage , weapon_info->armor_ratio , group );

						// set result data for when we hit a player.
						out->m_hitgroup = group;
						out->m_damage = player_damage;
						out->m_target = enter_trace.entity;

						// non-penetrate damage.
						if ( penetration_count == 4 )
							return player_damage >= in->m_damage;

						// penetration damage.
						return player_damage >= in->m_damage_pen;
					}

					// if we've reached here then we didn't hit a player yet, set damage and hitgroup.
					out->m_damage = current_damage;
				}

				// don't run pen code if it's not wanted.
				if ( !in->m_can_pen )
					return false;

				// check if we reach penetration distance, no more penetrations after that
				// or if our modifier is super low, just stop the bullet.
				if ( ( current_distance > penetration_distance && penetration > 0.f ) ||
					penetration_modifier < 0.1f )
				{
					penetration_count = 0;
					break;
				}

				bool bullet_stopped = handle_bullet_penetration(
					in->m_from ,
					penetration ,
					enter_material ,
					enter_trace ,
					dir ,
					penetration_modifier ,
					damage_modifier ,
					penetration_power ,
					penetration_count ,
					start ,
					distance ,
					current_distance ,
					current_damage );

				if ( bullet_stopped )
					break;
			}

			return false;
		}

		bool run( base_entity* target , vec3_t point , float dmg , penetration_output_t* out )
		{
			penetration_input_t in;
			in.m_damage = in.m_damage_pen = ( float ) dmg;
			in.m_can_pen = true;
			in.m_target = target;
			in.m_from = g::local;
			in.m_pos = point;

			return run( &in , out );
		}

	}
}