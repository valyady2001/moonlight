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


float DotProduct( const vec3_t& a, const vec3_t& b ) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DistanceToRay( const vec3_t& pos, const vec3_t& rayStart, const vec3_t& rayEnd, float* along = NULL, vec3_t* pointOnRay = NULL )
{
	vec3_t to = pos - rayStart;
	vec3_t dir = rayEnd - rayStart;
	float length = dir.normalize_in_place( );

	float rangeAlong = DotProduct( dir, to );
	if (along)
	{
		*along = rangeAlong;
	}

	float range;

	if (rangeAlong < 0.0f)
	{
		// off start point
		range = -( pos - rayStart ).length( );

		if (pointOnRay)
		{
			*pointOnRay = rayStart;
		}
	}
	else if (rangeAlong > length)
	{
		// off end point
		range = -( pos - rayEnd ).length( );

		if (pointOnRay)
		{
			*pointOnRay = rayEnd;
		}
	}
	else // within ray bounds
	{
		vec3_t onRay = rayStart + rangeAlong * dir;
		range = ( pos - onRay ).length( );

		if (pointOnRay)
		{
			*pointOnRay = onRay;
		}
	}

	return range;
}

void util_clip_trace_to_players( const vec3_t& start, const vec3_t& end, unsigned int mask, trace_filter* filter, trace_t* trace, float range = 0, float fraction = -1 )
{
	trace_t player_trace;
	ray_t ray;
	if (fraction < 0.0f)
		fraction = trace->flFraction;
	const float maxRange = 60.0f;

	ray.initialize( start, end );

	for (int i = 0; i <= 64; i++)
	{
		base_entity* player = ( base_entity* ) interfaces::ent_list->get_client_entity( i );

		if (!player || !player->is_alive( ))
			continue;

		if (player->dormant( ))
			continue;

		if (filter && filter->ShouldHitEntity( player, mask ) == false)
			continue;

		float m_range = DistanceToRay( player->WorldSpaceCenter( ), start, end );

		if (m_range < range || m_range > maxRange)
			continue;

		interfaces::trace_ray->clip_ray_to_entity( ray, mask | contents_hitbox, player, &player_trace );
		if (player_trace.flFraction < fraction)
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
		float scale( base_entity* player, float& damage, float armor_ratio, int hitgroup )
		{
			bool  has_heavy_armor;
			int   armor;
			float heavy_ratio, bonus_ratio, ratio, new_damage;

			auto is_armored = [ ] ( base_entity* player, int armor, int hitgroup ) {
				// the player has no armor.
				if (armor <= 0)
					return false;

				// if the hitgroup is head and the player has a helment, return true.
				// otherwise only return true if the hitgroup is not generic / legs / gear.
				if (hitgroup == hitgroup_head && player->has_helmet( ))
					return true;

				else if (hitgroup >= hitgroup_chest && hitgroup <= hitgroup_rightarm)
					return true;

				return false;
			};

			// check if the player has heavy armor, this is only really used in operation stuff.
			has_heavy_armor = false; // player->has_heavy_armor( );

			// scale damage based on hitgroup.
			switch (hitgroup) {
			case hitgroup_head:
				if (has_heavy_armor)
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
			if (is_armored( player, armor, hitgroup )) {
				heavy_ratio = 1.f;
				bonus_ratio = 0.5f;
				ratio = armor_ratio * 0.5f;

				// player has heavy armor.
				if (has_heavy_armor) {
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

				if (( ( damage - new_damage ) * ( heavy_ratio * bonus_ratio ) ) > armor)
					new_damage = damage - ( armor / bonus_ratio );

				damage = new_damage;
			}

			return damage;
		}
		bool breakable( base_entity* e )
		{
			//static auto IsBreakableEntity = ( bool( __thiscall* )( base_entity* ) )( utilities::find_sig( "client.dll" , "55 8B EC 51 56 8B F1 85 F6 74 68" ) );

			//bool        ret;
			//client_class_t* cc;
			//const char* name;
			//char* takedmg , old_takedmg;

			//static size_t m_takedamage_offset{ *( size_t* ) ( ( uintptr_t ) IsBreakableEntity + 0x26 ) };

			//// skip null ents and the world ent.
			//if ( !e || e->idx( ) == 0 )
			//	return false;

			//// get m_takedamage and save old m_takedamage.
			//takedmg = ( char* ) ( ( uintptr_t ) e + m_takedamage_offset );
			//old_takedmg = *takedmg;

			//// get clientclass.
			//cc = e->client_class( );

			//if ( cc ) {
			//	// get clientclass network name.
			//	name = cc->network_name;

			//	// CBreakableSurface, CBaseDoor, ...
			//	if ( name[ 1 ] != 'F'
			//		|| name[ 4 ] != 'c'
			//		|| name[ 5 ] != 'B'
			//		|| name[ 9 ] != 'h' ) {
			//		*takedmg = DAMAGE_YES;
			//	}
			//}

			//ret = IsBreakableEntity( e );
			//*takedmg = old_takedmg;
			//return ret;

			if (!e || !e->idx( ))
				return false;
			int iHealth = e->health( );
			if (e->GetTakeDamage( ) != DAMAGE_YES)
				return false;
			int CollisionGroup = e->GetCollisionGroup( );
			if (CollisionGroup != COLLISION_GROUP_PUSHAWAY && CollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && CollisionGroup != COLLISION_GROUP_NONE)
				return false;
			if (iHealth > 200)
				return false;
			IMultiplayerPhysics* pPhysicsInterface = ( IMultiplayerPhysics* ) e;
			if (pPhysicsInterface) {
				if (pPhysicsInterface->GetMultiplayerPhysicsMode( ) != PHYSICS_MULTIPLAYER_SOLID)
					return false;
			}
			else
			{
				const char* szClassname = e->GetClassname( );
				if (szClassname [ 0 ] == 'f' && szClassname [ 5 ] == 'b' && szClassname [ 13 ] == 'e')
				{
					if (szClassname [ 15 ] == 's')
					{
						// don't try to break it if it has already been broken
						if (e->is_broken( ))
							return false;
					}
				}
				else if (e->PhysicsSolidMaskForEntity( ) & contents_playerclip)
				{
					// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
					return false;
				}
			}
			IBreakableWithPropData* pBreakableInterface = ( IBreakableWithPropData* ) e;
			if (pBreakableInterface)
				if (pBreakableInterface->GetDmgModBullet( ) <= 0.0f)
					return false;

			return true;
		}
		bool trace_to_exit( const vec3_t& start, const vec3_t& dir, vec3_t& out, trace_t* enter_trace, trace_t* exit_trace )
		{
			static trace_filter_simple_game_t filter {};

			float dist = 0.0f;
			vec3_t new_end;
			int contents, first_contents = 0;
			ray_t ray;

			// max pen distance is 90 units.
			while (dist <= 90.f) {
				// step forward a bit.
				dist += 4.f;

				// set out pos.
				out = start + ( dir * dist );

				contents = interfaces::trace_ray->get_point_contents( out, mask_shot, nullptr );

				if (!first_contents)
					first_contents = contents;

				if (!( contents & mask_shot_hull ) || contents & contents_hitbox && contents != first_contents) {

					// move end pos a bit for tracing.
					new_end = out - ( dir * 4.f );

					// do first trace.
					ray.initialize( out, new_end );
					interfaces::trace_ray->trace_ray( ray, mask_shot, nullptr, exit_trace );

					// note; this is some new stuff added sometime around late 2017 ( 10.31.2017 update? ).
					util_clip_trace_to_players( out, new_end, mask_shot, nullptr, exit_trace, -60.f );

					// we hit an ent's hitbox, do another trace.
					if (exit_trace->startSolid && ( exit_trace->surface.flags & surf_hitbox ) != 0) {
						ray.initialize( out, start );
						filter.set_pass_entity( exit_trace->entity );
						interfaces::trace_ray->trace_ray( ray, mask_shot_hull, ( trace_filter* ) &filter, exit_trace );

						if (exit_trace->did_hit( ) && !exit_trace->startSolid) {
							out = exit_trace->end;
							return true;
						}

					}
					else {
						if (!exit_trace->did_hit( ) || exit_trace->startSolid) {
							if (enter_trace->DidHitNonWorldEntity( ) && ( breakable( enter_trace->entity ) || enter_trace->entity->client_class( )->class_id == EClassIds::CBaseEntity )) {
								*exit_trace = *enter_trace;
								exit_trace->end = start + dir;
								return true;
							}

						}
						else {
							if (( exit_trace->surface.flags & surf_nodraw )) {
								// note; server doesnt seem to check CGameTrace::DidHitNonWorldEntity for both of these breakable checks?
								if (breakable( exit_trace->entity ) && breakable( enter_trace->entity )) {
									out = exit_trace->end;
									return true;
								}

								if (!( enter_trace->surface.flags & surf_nodraw ))
									continue;
							}

							if (exit_trace->plane.normal.dot( dir ) <= 1.f) {
								out -= ( dir * ( exit_trace->flFraction * 4.f ) );
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		void clip_trace_to_player( const vec3_t& start, const vec3_t& end, uint32_t mask, trace_t* tr, base_entity* player, float min )
		{
			vec3_t pos, to, dir, on_ray;
			float len, range_along, range;
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
			if (range_along < 0.f)
				range = -( to ).length( );

			// off end point.
			else if (range_along > len)
				range = -( pos - end ).length( );

			// within ray bounds.
			else {
				on_ray = start + ( dir * range_along );
				range = ( pos - on_ray ).length( );
			}

			if ( /*min <= range &&*/ range <= 60.f) {
				// clip to player.
				ray_t m_clip_trace; m_clip_trace.initialize( start, end );
				interfaces::trace_ray->clip_ray_to_entity( m_clip_trace, mask, player, &new_trace );

				if (tr->flFraction > new_trace.flFraction)
					*tr = new_trace;
			}
		}
		bool is_valid_hitgroup( int idx )
		{
			if (( idx >= hitgroup_head && idx <= hitgroup_rightleg ) || idx == hitgroup_gear)
				return true;

			return false;
		}

		bool run( penetration_input_t* in, penetration_output_t* out )
		{
			static trace_filter_skip_two_entities_game_t filter {};

			int pen { 4 }, enter_material, exit_material;
			float damage, penetration, penetration_mod, player_damage, remaining, trace_len {}, total_pen_mod, damage_mod, damage_lost_percent, modifier, damage_lost;
			surfacedata_t* enter_surface, * exit_surface;
			bool nodraw, grate;
			vec3_t start, dir, end, pen_end;
			trace_t trace, exit_trace;
			c_weapon* weapon;
			weapon_info_t* weapon_info;
			ray_t ray;
			base_entity* last_hit_entity = nullptr;

			// if we are tracing from our local player perspective.
			if (in->m_from == g::local) {
				weapon = g::local->weapon( );
				weapon_info = g::local->weapon( )->weapon_data( );
				start = g::eye_pos;
			}

			// not local player.
			else {
				weapon = in->m_from->weapon( );
				if (!weapon)
					return false;

				// get weapon info.
				weapon_info = weapon->weapon_data( );
				if (!weapon_info)
					return false;

				// set trace start.
				start = in->m_from->weapon_shoot_pos( );
			}

			// get some weapon data.
			damage = ( float ) weapon_info->dmg;
			penetration = weapon_info->penetration;

			// used later in calculations.
			penetration_mod = std::fmaxf( 0.f, ( 3.f / penetration ) * 1.25f );

			// get direction to end point.
			dir = math::angle_1vec( math::calculate_angle( start, in->m_pos ) );

			// setup trace filter for later.
			filter.set_pass_entity( in->m_from );
			filter.set_pass_entity2( nullptr );

			while (damage > 0.f) {
				// calculating remaining len.
				remaining = weapon_info->range - trace_len;

				// set trace end.
				end = start + ( dir * remaining );

				// setup ray and trace.
				ray.initialize( start, end );
				filter.set_pass_entity( in->m_from );
				filter.set_pass_entity2( last_hit_entity ? last_hit_entity : nullptr );
				interfaces::trace_ray->trace_ray( ray, mask_shot | contents_hitbox, ( trace_filter* ) &filter, &trace );

				// we didn't hit anything.
				if (trace.flFraction == 1.f)
					return false;

				// check for player hitboxes extending outside their collision bounds.
				// if no target is passed we clip the trace to a specific player, otherwise we clip the trace to any player.
				if (in->m_target)
					clip_trace_to_player( start, end + ( dir * 40.f ), mask_shot | contents_hitbox, &trace, in->m_target, -60.f );
				else
					util_clip_trace_to_players( start, end + ( dir * 40.f ), mask_shot | contents_hitbox, ( trace_filter* ) &filter, &trace, -60.f );

				// calculate damage based on the distance the bullet traveled.
				trace_len += trace.flFraction * remaining;
				damage *= std::pow( weapon_info->range_modifier, trace_len / 500.f );

				// get last hit entity for trace.
				last_hit_entity = trace.entity ? ( ( base_entity* ) ( trace.entity ) )->is_player( ) ? trace.entity : nullptr : nullptr;
				// if a target was passed.
				if (in->m_target) {

					// validate that we hit the target we aimed for.
					if (trace.entity && trace.entity == in->m_target && is_valid_hitgroup( trace.hitGroup )) {
						int group = ( weapon->item_definition_index( ) == zeus ) ? hitgroup_generic : trace.hitGroup;

						// scale damage based on the hitgroup we hit.
						player_damage = scale( in->m_target, damage, weapon_info->armor_ratio, group );

						// set result data for when we hit a player.
						out->m_pen = pen != 4;
						out->m_hitgroup = group;
						out->m_damage = player_damage;
						out->m_target = in->m_target;

						// non-penetrate damage.
						if (pen == 4)
							return player_damage >= in->m_damage;

						// penetration damage.
						return player_damage >= in->m_damage_pen;
					}
				}

				// no target was passed, check for any player hit or just get final damage done.
				else {
					out->m_pen = pen != 4;

					// todo - dex; team checks / other checks / etc.
					if (trace.entity && ( ( base_entity* ) trace.entity )->is_player( ) && is_valid_hitgroup( trace.hitGroup )) {
						int group = ( weapon->item_definition_index( ) == zeus ) ? hitgroup_generic : trace.hitGroup;

						player_damage = scale( ( ( base_entity* ) trace.entity ), damage, weapon_info->armor_ratio, group );

						// set result data for when we hit a player.
						out->m_hitgroup = group;
						out->m_damage = player_damage;
						out->m_target = ( ( base_entity* ) trace.entity );

						// non-penetrate damage.
						if (pen == 4)
							return player_damage >= in->m_damage;

						// penetration damage.
						return player_damage >= in->m_damage_pen;
					}

					// if we've reached here then we didn't hit a player yet, set damage and hitgroup.
					out->m_damage = damage;
				}
				// don't run pen code if it's not wanted.
				if (!in->m_can_pen)
					return false;

				// get surface at entry point.
				enter_surface = interfaces::physics_props->get_surface_data( trace.surface.surfaceProps );

				// this happens when we're too far away from a surface and can penetrate walls or the surface's pen modifier is too low.
				if (( trace_len > 3000.f && penetration ) || enter_surface->game.flPenetrationModifier < 0.1f)
					return false;

				// store data about surface flags / contents.
				nodraw = ( trace.surface.flags & surf_nodraw );
				grate = ( trace.contents & contents_grate );

				// get material at entry point.
				enter_material = enter_surface->game.material;

				// note - dex; some extra stuff the game does.
				if (!pen && !nodraw && !grate && enter_material != char_tex_grate && enter_material != char_tex_glass)
					return false;

				// no more pen.
				if (penetration <= 0.f || pen <= 0)
					return false;

				// try to penetrate object.
				if (!trace_to_exit( trace.end, dir, pen_end, &trace, &exit_trace )) {
					if (!( interfaces::trace_ray->get_point_contents( pen_end, mask_shot_hull ) & mask_shot_hull ))
						return false;
				}

				// get surface / material at exit point.
				exit_surface = interfaces::physics_props->get_surface_data( exit_trace.surface.surfaceProps );
				exit_material = exit_surface->game.material;

				if(enter_material != char_tex_flesh)
				damage_mod = 0.99f;
				/*else if (enter_material == char_tex_flesh && trace.entity && trace.entity->is_player( ) && trace.entity->team( ) != g::local->team( ))
				{
					penetration_mod = penetration;
					damage_mod = penetration;

				}*/
				// todo - dex; check for CHAR_TEX_FLESH and ff_damage_bullet_penetration / ff_damage_reduction_bullets convars?
				//             also need to check !isbasecombatweapon too.
				if (enter_material == char_tex_grate || enter_material == char_tex_glass) {
					total_pen_mod = 3.f;
					damage_lost_percent = 0.05f;
				}

				else if (nodraw || grate) {
					total_pen_mod = 1.f;
					
				}

				else {
					total_pen_mod = ( enter_surface->game.flPenetrationModifier + exit_surface->game.flPenetrationModifier ) * 0.49f;
					damage_lost_percent = 0.16f;
				}

				// thin metals, wood and plastic get a penetration bonus.
				if (enter_material == exit_material) {
					if (exit_material == char_tex_cardboard || exit_material == char_tex_wood)
						total_pen_mod = 3.f;

					else if (exit_material == char_tex_plastic)
						total_pen_mod = 2.f;
				}

				// set some local vars.
				trace_len = ( exit_trace.end - trace.end ).length( );
				modifier = fmaxf( 1.f / total_pen_mod, 0.f );


				// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
				damage_lost =
					( ( ( damage * damage_lost_percent )
						+ ( fmaxf( 3.0f / penetration * 1.25f, 0.f ) * 3.f * modifier ) )*( trace_len * trace_len ) / 24.f  )
					+ ( ( damage * damage_lost_percent )
						+ ( fmaxf( 3.0f / penetration * 1.25f, 0.f ) * 3.f * modifier ) );


				// subtract from damage.
				damage -= std::fmaxf( 0.f, damage_lost );
				damage *= damage_mod;
				if (damage <= 1.f)
					return false;

				// set new start pos for successive trace.
				start = exit_trace.end;

				// decrement pen.
				--pen;
			}

			return false;
		}

		bool run( base_entity* target, vec3_t point, float dmg, penetration_output_t* out )
		{
			penetration_input_t in;
			in.m_damage = in.m_damage_pen = ( float ) dmg;
			in.m_can_pen = true;
			in.m_target = target;
			in.m_from = g::local;
			in.m_pos = point;

			return run( &in, out );
		}

	}
}