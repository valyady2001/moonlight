#include "features.h"
double to_180( double x ) {
	x = fmod( x + 180, 360 );
	if (x < 0)
		x += 360;
	return x - 180;
}


vec3_t GetSmoothedVelocity( float min_delta, vec3_t a, vec3_t b ) {
	vec3_t delta = a - b;
	float delta_length = delta.length( );

	if (delta_length <= min_delta) {
		vec3_t result;
		if (-min_delta <= delta_length) {
			return a;
		}
		else {
			float iradius = 1.0f / ( delta_length + FLT_EPSILON );
			return b - ( ( delta * iradius ) * min_delta );
		}
	}
	else {
		float iradius = 1.0f / ( delta_length + FLT_EPSILON );
		return b + ( ( delta * iradius ) * min_delta );
	}
}

float hitbox_fraction( base_entity* e, int hb )
{
	trace_t trace;
	vec3_t start = g::local->eye_pos( );
	ray_t ray;
	trace_filter filter;
	vec3_t end = e->get_hitbox_pos( hb );
	ray.initialize( start, end );
	filter.skip = g::local;
	interfaces::trace_ray->trace_ray( ray, mask_visible, &filter, &trace );

	return trace.flFraction;
}

double to_360( double x ) {
	x = fmod( x, 360 );
	if (x < 0)
		x += 360;
	return x;
}

void create_layer( base_entity* pl, animlayer layer [ 13 ], int delta )
{
	if (pl->team( ) == g::local->team( ))
		return;

	animlayer backup_layers [ 13 ];
	pl->GetAnimLayers( backup_layers );

	animstate state;
	memcpy( &state, pl->get_animstate( ), sizeof( animstate ) );

	const auto backup_occlusion_flags = pl->m_iOcclusionFlags( );
	const auto backup_occlusion_framecount = pl->m_iOcclusionFramecount( );

	// skip occlusion checks in c_cs_player::setup_bones.
	if (pl != g::local)
	{
		pl->m_iOcclusionFlags( ) = 0;
		pl->m_iOcclusionFramecount( ) = 0;
	}

	// clear bone accessor.
	pl->GetReadableBones( ) = pl->GetWriteableBones( ) = 0;

	// invalidate bone cache.
	pl->GetMostRecentModelBoneCounter( ) = 0;
	pl->GetLastBoneSetupTime( ) = -FLT_MAX;

	// stop interpolation.
	pl->effects( ) |= 8;
	pl->get_animstate( )->abs_yaw = pl->get_animstate( )->eye_yaw + delta;
	pl->update_animations( );

	memcpy( layer, pl->anim_layers( ), sizeof( animlayer ) * 13 );

	// build bones.
	//setup_bones( pl , bone_used_by_anything & ~bone_used_by_attachment , pl->simtime( ) , bones );

	pl->SetAnimLayers( backup_layers );
	memcpy( pl->get_animstate( ), &state, sizeof( animstate ) );

	// restore original occlusion.
	if (pl != g::local)
	{
		pl->m_iOcclusionFlags( ) = backup_occlusion_flags;
		pl->m_iOcclusionFramecount( ) = backup_occlusion_framecount;
	}

	// start interpolation again.
	pl->effects( ) &= ~8;
}
namespace features
{
	namespace resolver
	{
		int side( base_entity* e )
		{
			if (!e || !e->is_alive( ))
				return 0;

			auto head = e->get_hitbox_pos( hitboxes::head );

			auto left = head + vec3_t( 0.f, 90.f, 0.f );
			auto right = head - vec3_t( 0.f, 90.f, 0.f );

			trace_t trace_left;
			trace_t trace_right;
			trace_filter filter;
			filter.skip = e;
			vec3_t pos = e->eye_pos( );
			pos.x = 0;
			ray_t ray_left;
			ray_left.initialize( pos, left );
			interfaces::trace_ray->trace_ray( ray_left, mask_visible, &filter, &trace_left );

			ray_t ray_right;
			ray_right.initialize( pos, right );
			interfaces::trace_ray->trace_ray( ray_right, mask_visible, &filter, &trace_right );

			if (trace_left.flFraction < trace_right.flFraction)
				return 1;
			else
				return -1;

			return 0;
		}

		void resolve( base_entity* e )
		{
			if (!interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ))
				return;
			if (!g::local || !g::local->is_alive( ))
				return;
			if (!e || !e->is_alive( ) || e == g::local || e->team( ) == g::local->team( ) || !e->is_player( ))
			{
				if (e)
					ragebot::misses [ e->idx( ) ] = 0;
				return;
			}
			auto i = e->idx( );

			if (ragebot::misses [ i ] > 7)
				ragebot::misses [ i ] = 0;

			animstate* o_state;
			o_state = e->get_animstate( );
			auto body_yaw = e->get_animstate( )->eye_yaw - e->get_animstate( )->abs_yaw;
			resolver_info [ e->idx( ) ].hit_abs = e->get_animstate( )->abs_yaw;
			if (ragebot::hitted [ i ] && ragebot::misses [ i ] == 0) {
				{//am dat hit la satean ii dam bully acum
					resolver_info [ e->idx( ) ].hit_abs = e->get_animstate( )->abs_yaw;
					resolver_info [ e->idx( ) ].hit_state = *e->get_animstate( );
				}
			}
			else resolver_info [ e->idx( ) ].hit_abs = e->get_animstate()->abs_yaw;


			if (!e->get_animstate( ))
				return;
			if (e->get_animstate( )->eye_yaw == resolver_info [ e->idx( ) ].hit_state.eye_yaw)
			{
				resolver_info [ e->idx( ) ].resolved_abs = resolver_info [ e->idx( ) ].hit_abs;

			}
			else {

				if (ragebot::misses [ i ] < 1) {
					if (e->velocity( ).length_2d( ) <= 6.f && e->anim_layer( 3 )->cycle == 0.f && e->anim_layer( 3 )->weight == 0.f &&e->anim_layer(3)->sequence == 979 && e->flags( ) & fl_onground) {

						if (e->lower_body_yaw( ) > 0.f)
							resolver_info [ e->idx( ) ].resolved_abs =  e->get_animstate( )->eye_yaw + ( e->lower_body_yaw() / ( -60.f ) ) *( e->get_animstate( )->max_yaw/2.f );
						if (e->lower_body_yaw( ) < 0.f)
							resolver_info [ e->idx( ) ].resolved_abs =  e->get_animstate( )->eye_yaw + ( e->lower_body_yaw( ) / ( 60.f ) ) * (e->get_animstate( )->min_yaw/2.f );
						else if (e->lower_body_yaw( ) == 0)
							resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw;
					}
					else if (e->velocity( ).length_2d( ) > 0.f && e->velocity( ).length_2d( ) < 150.f && e->flags( ) & fl_onground)
					{
						if (body_yaw > 0.f)
							resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + ( body_yaw / e->get_animstate( )->max_yaw ) * ( -60.f ) ;
						if (body_yaw < 0.f)
							resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + ( body_yaw / e->get_animstate( )->min_yaw ) * ( 60.f ) ;
						else if (body_yaw == 0)
							resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw;
					}
					else if (e->velocity( ).length_2d( ) > 150.f && e->flags( ) & fl_onground)
					{
						resolver_info [ e->idx( ) ].resolved_abs =  e->lower_body_yaw( ) ;
					}
					else if (e->velocity( ).length_2d( ) > 150.f && !( e->flags( ) & fl_onground ))
					{
						resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw ;
					}
				}switch (ragebot::misses [ i ] % 4)
				{
				case 1:resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + side( e ) * 29.f ;
					break;
				case 2:resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + side( e ) * 58.f ;

					break;
				case 3:resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + side( e ) * ( -29.f ) ;
					break;
				case 4:resolver_info [ e->idx( ) ].resolved_abs = o_state->eye_yaw + side( e ) * ( -58.f ) ;
					break;
				}
			}
			e->get_animstate( )->abs_yaw = resolver_info [ e->idx( ) ].resolved_abs;
		}
	}
}