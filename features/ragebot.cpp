#include "features.h"
#include <sstream>
using namespace features;
using namespace lagcomp;

void set_config( ) {
	if ( !g::local || !interfaces::engine->is_connected( ) || !g::local->weapon( ) )
		return;
	if ( g::local->weapon( )->item_definition_index( ) == awp )//awp
	{

		g_var.ragebot.safepoint_type = g_var.awp.safepoint_type;
		g_var.ragebot.min_dmg = g_var.awp.min_dmg;
		g_var.ragebot.dmg_override = g_var.awp.dmg_override;
		g_var.ragebot.dmg_bind [ 0 ] = g_var.awp.dmg_bind [ 0 ];
		g_var.ragebot.dmg_bind [ 1 ] = g_var.awp.dmg_bind [ 1 ];
		g_var.ragebot.forceb_bind [ 0 ] = g_var.awp.forceb_bind [ 0 ];
		g_var.ragebot.forceb_bind [ 1 ] = g_var.awp.forceb_bind [ 1 ];
		g_var.ragebot.override_dmg = g_var.awp.override_dmg;
		g_var.ragebot.hitboxes [ 0 ] = g_var.awp.hitboxes [ 0 ];
		g_var.ragebot.hitboxes [ 1 ] = g_var.awp.hitboxes [ 1 ];
		g_var.ragebot.hitboxes [ 2 ] = g_var.awp.hitboxes [ 2 ];
		g_var.ragebot.hitboxes [ 3 ] = g_var.awp.hitboxes [ 4 ];
		g_var.ragebot.hitboxes [ 4 ] = g_var.awp.hitboxes [ 4 ];
		g_var.ragebot.hitboxes [ 5 ] = g_var.awp.hitboxes [ 5 ];
		g_var.ragebot.baim [ 0 ] = g_var.awp.baim [ 0 ];
		g_var.ragebot.baim [ 1 ] = g_var.awp.baim [ 1 ];
		g_var.ragebot.baim [ 2 ] = g_var.awp.baim [ 2 ];
		g_var.ragebot.baim [ 3 ] = g_var.awp.baim [ 3 ];
		g_var.ragebot.refine_shot = g_var.awp.refine_shot;
		g_var.ragebot.hitchance = g_var.awp.hitchance;
		g_var.ragebot.autostop_mode = g_var.awp.autostop_mode;
		g_var.ragebot.exploit = g_var.awp.exploit;
		g_var.ragebot.exploit_bind [ 0 ] = g_var.awp.exploit_bind [ 0 ];
		g_var.ragebot.exploit_bind [ 1 ] = g_var.awp.exploit_bind [ 1 ];
	}
	else
		if ( g::local->weapon( )->item_definition_index( ) == ssg08 )//scout
		{

			g_var.ragebot.safepoint_type = g_var.scout.safepoint_type;
			g_var.ragebot.min_dmg = g_var.scout.min_dmg;
			g_var.ragebot.dmg_override = g_var.scout.dmg_override;
			g_var.ragebot.dmg_bind [ 0 ] = g_var.scout.dmg_bind [ 0 ];
			g_var.ragebot.dmg_bind [ 1 ] = g_var.scout.dmg_bind [ 1 ];
			g_var.ragebot.forceb_bind [ 0 ] = g_var.scout.forceb_bind [ 0 ];
			g_var.ragebot.forceb_bind [ 1 ] = g_var.scout.forceb_bind [ 1 ];
			g_var.ragebot.override_dmg = g_var.scout.override_dmg;
			g_var.ragebot.hitboxes [ 0 ] = g_var.scout.hitboxes [ 0 ];
			g_var.ragebot.hitboxes [ 1 ] = g_var.scout.hitboxes [ 1 ];
			g_var.ragebot.hitboxes [ 2 ] = g_var.scout.hitboxes [ 2 ];
			g_var.ragebot.hitboxes [ 3 ] = g_var.scout.hitboxes [ 4 ];
			g_var.ragebot.hitboxes [ 4 ] = g_var.scout.hitboxes [ 4 ];
			g_var.ragebot.hitboxes [ 5 ] = g_var.scout.hitboxes [ 5 ];
			g_var.ragebot.baim [ 0 ] = g_var.scout.baim [ 0 ];
			g_var.ragebot.baim [ 1 ] = g_var.scout.baim [ 1 ];
			g_var.ragebot.baim [ 2 ] = g_var.scout.baim [ 2 ];
			g_var.ragebot.baim [ 3 ] = g_var.scout.baim [ 3 ];
			g_var.ragebot.refine_shot = g_var.scout.refine_shot;
			g_var.ragebot.hitchance = g_var.scout.hitchance;
			g_var.ragebot.autostop_mode = g_var.scout.autostop_mode;
			g_var.ragebot.exploit = g_var.scout.exploit;
			g_var.ragebot.exploit_bind [ 0 ] = g_var.scout.exploit_bind [ 0 ];
			g_var.ragebot.exploit_bind [ 1 ] = g_var.scout.exploit_bind [ 1 ];
		}
		else
			if ( g::local->weapon( )->item_definition_index( ) == scar20 || g::local->weapon( )->item_definition_index( ) == g3sg1 )//auto
			{

				g_var.ragebot.safepoint_type = g_var.autosniper.safepoint_type;
				g_var.ragebot.min_dmg = g_var.autosniper.min_dmg;
				g_var.ragebot.dmg_override = g_var.autosniper.dmg_override;
				g_var.ragebot.dmg_bind [ 0 ] = g_var.autosniper.dmg_bind [ 0 ];
				g_var.ragebot.dmg_bind [ 1 ] = g_var.autosniper.dmg_bind [ 1 ];
				g_var.ragebot.forceb_bind [ 0 ] = g_var.autosniper.forceb_bind [ 0 ];
				g_var.ragebot.forceb_bind [ 1 ] = g_var.autosniper.forceb_bind [ 1 ];
				g_var.ragebot.override_dmg = g_var.autosniper.override_dmg;
				g_var.ragebot.hitboxes [ 0 ] = g_var.autosniper.hitboxes [ 0 ];
				g_var.ragebot.hitboxes [ 1 ] = g_var.autosniper.hitboxes [ 1 ];
				g_var.ragebot.hitboxes [ 2 ] = g_var.autosniper.hitboxes [ 2 ];
				g_var.ragebot.hitboxes [ 3 ] = g_var.autosniper.hitboxes [ 4 ];
				g_var.ragebot.hitboxes [ 4 ] = g_var.autosniper.hitboxes [ 4 ];
				g_var.ragebot.hitboxes [ 5 ] = g_var.autosniper.hitboxes [ 5 ];
				g_var.ragebot.baim [ 0 ] = g_var.autosniper.baim [ 0 ];
				g_var.ragebot.baim [ 1 ] = g_var.autosniper.baim [ 1 ];
				g_var.ragebot.baim [ 2 ] = g_var.autosniper.baim [ 2 ];
				g_var.ragebot.baim [ 3 ] = g_var.autosniper.baim [ 3 ];
				g_var.ragebot.refine_shot = g_var.autosniper.refine_shot;
				g_var.ragebot.hitchance = g_var.autosniper.hitchance;
				g_var.ragebot.autostop_mode = g_var.autosniper.autostop_mode;
				g_var.ragebot.exploit = g_var.autosniper.exploit;
				g_var.ragebot.exploit_bind [ 0 ] = g_var.autosniper.exploit_bind [ 0 ];
				g_var.ragebot.exploit_bind [ 1 ] = g_var.autosniper.exploit_bind [ 1 ];
			}
			else
				if ( g::local->weapon( )->is_heavy( ) )//heavy pis
				{

					g_var.ragebot.safepoint_type = g_var.heavy.safepoint_type;
					g_var.ragebot.min_dmg = g_var.heavy.min_dmg;
					g_var.ragebot.dmg_override = g_var.heavy.dmg_override;
					g_var.ragebot.dmg_bind [ 0 ] = g_var.heavy.dmg_bind [ 0 ];
					g_var.ragebot.dmg_bind [ 1 ] = g_var.heavy.dmg_bind [ 1 ];
					g_var.ragebot.forceb_bind [ 0 ] = g_var.heavy.forceb_bind [ 0 ];
					g_var.ragebot.forceb_bind [ 1 ] = g_var.heavy.forceb_bind [ 1 ];
					g_var.ragebot.override_dmg = g_var.heavy.override_dmg;
					g_var.ragebot.hitboxes [ 0 ] = g_var.heavy.hitboxes [ 0 ];
					g_var.ragebot.hitboxes [ 1 ] = g_var.heavy.hitboxes [ 1 ];
					g_var.ragebot.hitboxes [ 2 ] = g_var.heavy.hitboxes [ 2 ];
					g_var.ragebot.hitboxes [ 3 ] = g_var.heavy.hitboxes [ 4 ];
					g_var.ragebot.hitboxes [ 4 ] = g_var.heavy.hitboxes [ 4 ];
					g_var.ragebot.hitboxes [ 5 ] = g_var.heavy.hitboxes [ 5 ];
					g_var.ragebot.baim [ 0 ] = g_var.heavy.baim [ 0 ];
					g_var.ragebot.baim [ 1 ] = g_var.heavy.baim [ 1 ];
					g_var.ragebot.baim [ 2 ] = g_var.heavy.baim [ 2 ];
					g_var.ragebot.baim [ 3 ] = g_var.heavy.baim [ 3 ];
					g_var.ragebot.refine_shot = g_var.heavy.refine_shot;
					g_var.ragebot.hitchance = g_var.heavy.hitchance;
					g_var.ragebot.autostop_mode = g_var.heavy.autostop_mode;
					g_var.ragebot.exploit = g_var.heavy.exploit;
					g_var.ragebot.exploit_bind [ 0 ] = g_var.heavy.exploit_bind [ 0 ];
					g_var.ragebot.exploit_bind [ 1 ] = g_var.heavy.exploit_bind [ 1 ];
				}
				else
					if ( g::local->weapon( )->is_pistol( ) )//pistols
					{

						g_var.ragebot.safepoint_type = g_var.pistols.safepoint_type;
						g_var.ragebot.min_dmg = g_var.pistols.min_dmg;
						g_var.ragebot.dmg_override = g_var.pistols.dmg_override;
						g_var.ragebot.dmg_bind [ 0 ] = g_var.pistols.dmg_bind [ 0 ];
						g_var.ragebot.dmg_bind [ 1 ] = g_var.pistols.dmg_bind [ 1 ];
						g_var.ragebot.forceb_bind [ 0 ] = g_var.pistols.forceb_bind [ 0 ];
						g_var.ragebot.forceb_bind [ 1 ] = g_var.pistols.forceb_bind [ 1 ];
						g_var.ragebot.override_dmg = g_var.pistols.override_dmg;
						g_var.ragebot.hitboxes [ 0 ] = g_var.pistols.hitboxes [ 0 ];
						g_var.ragebot.hitboxes [ 1 ] = g_var.pistols.hitboxes [ 1 ];
						g_var.ragebot.hitboxes [ 2 ] = g_var.pistols.hitboxes [ 2 ];
						g_var.ragebot.hitboxes [ 3 ] = g_var.pistols.hitboxes [ 4 ];
						g_var.ragebot.hitboxes [ 4 ] = g_var.pistols.hitboxes [ 4 ];
						g_var.ragebot.hitboxes [ 5 ] = g_var.pistols.hitboxes [ 5 ];
						g_var.ragebot.baim [ 0 ] = g_var.pistols.baim [ 0 ];
						g_var.ragebot.baim [ 1 ] = g_var.pistols.baim [ 1 ];
						g_var.ragebot.baim [ 2 ] = g_var.pistols.baim [ 2 ];
						g_var.ragebot.baim [ 3 ] = g_var.pistols.baim [ 3 ];
						g_var.ragebot.refine_shot = g_var.pistols.refine_shot;
						g_var.ragebot.hitchance = g_var.pistols.hitchance;
						g_var.ragebot.autostop_mode = g_var.pistols.autostop_mode;
						g_var.ragebot.exploit = g_var.pistols.exploit;
						g_var.ragebot.exploit_bind [ 0 ] = g_var.pistols.exploit_bind [ 0 ];
						g_var.ragebot.exploit_bind [ 1 ] = g_var.pistols.exploit_bind [ 1 ];
					}
					else
						if ( g::local->weapon( )->is_smg( ) )//smg
						{

							g_var.ragebot.safepoint_type = g_var.smgs.safepoint_type;
							g_var.ragebot.min_dmg = g_var.smgs.min_dmg;
							g_var.ragebot.dmg_override = g_var.smgs.dmg_override;
							g_var.ragebot.dmg_bind [ 0 ] = g_var.smgs.dmg_bind [ 0 ];
							g_var.ragebot.dmg_bind [ 1 ] = g_var.smgs.dmg_bind [ 1 ];
							g_var.ragebot.forceb_bind [ 0 ] = g_var.smgs.forceb_bind [ 0 ];
							g_var.ragebot.forceb_bind [ 1 ] = g_var.smgs.forceb_bind [ 1 ];
							g_var.ragebot.override_dmg = g_var.smgs.override_dmg;
							g_var.ragebot.hitboxes [ 0 ] = g_var.smgs.hitboxes [ 0 ];
							g_var.ragebot.hitboxes [ 1 ] = g_var.smgs.hitboxes [ 1 ];
							g_var.ragebot.hitboxes [ 2 ] = g_var.smgs.hitboxes [ 2 ];
							g_var.ragebot.hitboxes [ 3 ] = g_var.smgs.hitboxes [ 4 ];
							g_var.ragebot.hitboxes [ 4 ] = g_var.smgs.hitboxes [ 4 ];
							g_var.ragebot.hitboxes [ 5 ] = g_var.smgs.hitboxes [ 5 ];
							g_var.ragebot.baim [ 0 ] = g_var.smgs.baim [ 0 ];
							g_var.ragebot.baim [ 1 ] = g_var.smgs.baim [ 1 ];
							g_var.ragebot.baim [ 2 ] = g_var.smgs.baim [ 2 ];
							g_var.ragebot.baim [ 3 ] = g_var.smgs.baim [ 3 ];
							g_var.ragebot.refine_shot = g_var.smgs.refine_shot;
							g_var.ragebot.hitchance = g_var.smgs.hitchance;
							g_var.ragebot.autostop_mode = g_var.smgs.autostop_mode;
							g_var.ragebot.exploit = g_var.smgs.exploit;
							g_var.ragebot.exploit_bind [ 0 ] = g_var.smgs.exploit_bind [ 0 ];
							g_var.ragebot.exploit_bind [ 1 ] = g_var.smgs.exploit_bind [ 1 ];
						}
						else
							if ( g::local->weapon( )->is_rifle( ) ) //rifles
							{

								g_var.ragebot.safepoint_type = g_var.rifles.safepoint_type;
								g_var.ragebot.min_dmg = g_var.rifles.min_dmg;
								g_var.ragebot.dmg_override = g_var.rifles.dmg_override;
								g_var.ragebot.dmg_bind [ 0 ] = g_var.rifles.dmg_bind [ 0 ];
								g_var.ragebot.dmg_bind [ 1 ] = g_var.rifles.dmg_bind [ 1 ];
								g_var.ragebot.forceb_bind [ 0 ] = g_var.rifles.forceb_bind [ 0 ];
								g_var.ragebot.forceb_bind [ 1 ] = g_var.rifles.forceb_bind [ 1 ];
								g_var.ragebot.override_dmg = g_var.rifles.override_dmg;
								g_var.ragebot.hitboxes [ 0 ] = g_var.rifles.hitboxes [ 0 ];
								g_var.ragebot.hitboxes [ 1 ] = g_var.rifles.hitboxes [ 1 ];
								g_var.ragebot.hitboxes [ 2 ] = g_var.rifles.hitboxes [ 2 ];
								g_var.ragebot.hitboxes [ 3 ] = g_var.rifles.hitboxes [ 4 ];
								g_var.ragebot.hitboxes [ 4 ] = g_var.rifles.hitboxes [ 4 ];
								g_var.ragebot.hitboxes [ 5 ] = g_var.rifles.hitboxes [ 5 ];
								g_var.ragebot.baim [ 0 ] = g_var.rifles.baim [ 0 ];
								g_var.ragebot.baim [ 1 ] = g_var.rifles.baim [ 1 ];
								g_var.ragebot.baim [ 2 ] = g_var.rifles.baim [ 2 ];
								g_var.ragebot.baim [ 3 ] = g_var.rifles.baim [ 3 ];
								g_var.ragebot.refine_shot = g_var.rifles.refine_shot;
								g_var.ragebot.hitchance = g_var.rifles.hitchance;
								g_var.ragebot.autostop_mode = g_var.rifles.autostop_mode;
								g_var.ragebot.exploit = g_var.rifles.exploit;
								g_var.ragebot.exploit_bind [ 0 ] = g_var.rifles.exploit_bind [ 0 ];
								g_var.ragebot.exploit_bind [ 1 ] = g_var.rifles.exploit_bind [ 1 ];
							}
}
std::vector<int> hitboxes( base_entity* e , c_weapon* wpn )
{
	std::vector<int> HitBoxesToScan;
	bool allow_head = true;

	if ( g_var.ragebot.baim [ 0 ] )
		if ( e->velocity( ).length_2d( ) < 70.f && e->velocity( ).length_2d( ) > 30.f )
			allow_head = false;

	if ( g_var.ragebot.baim [ 1 ] )
		if ( wpn->weapon_data( )->dmg > e->health( ) )
			allow_head = false;

	if ( keyhandler::auto_check( g_var.ragebot.forceb_bind [ 1 ] , g_var.ragebot.forceb_bind [ 0 ] ) )
		allow_head = false;

	if ( g_var.ragebot.hitboxes [ 0 ] && allow_head )
		HitBoxesToScan.push_back( hitboxes::head );

	if ( g_var.ragebot.hitboxes [ 1 ] )
		HitBoxesToScan.push_back( hitboxes::chest );

	if ( g_var.ragebot.hitboxes [ 2 ] )
		HitBoxesToScan.push_back( hitboxes::stomach );

	if ( g_var.ragebot.hitboxes [ 3 ] )
		HitBoxesToScan.push_back( hitboxes::pelvis );

	if ( g_var.ragebot.hitboxes [ 4 ] )
	{
		HitBoxesToScan.push_back( hitboxes::left_upper_arm );
		HitBoxesToScan.push_back( hitboxes::right_upper_arm );
	}

	if ( g_var.ragebot.hitboxes [ 5 ] )
	{
		HitBoxesToScan.push_back( hitboxes::left_thigh );
		HitBoxesToScan.push_back( hitboxes::right_thigh );
	}

	if ( g_var.ragebot.hitboxes [ 6 ] )
	{
		HitBoxesToScan.push_back( hitboxes::right_foot );
		HitBoxesToScan.push_back( hitboxes::left_foot );
	}


	return HitBoxesToScan;
}

bool CanHitHitbox( const vec3_t start , const vec3_t end , records* record , base_entity* e , studiohdr_t* hdr , int box )//crash
{
	if ( !e )
		return false;
	if ( !record )
		return false;
	const auto backup_origin = record->orig;
	const auto backup_abs_origin = record->abs_orig;
	const auto backup_abs_angles = record->abs_ang;
	const auto backup_obb_mins = record->mins;
	const auto backup_obb_maxs = record->maxs;
	const auto backup_cache = e->bone_cache( );
	auto matrix = record->bones;
	studiohdr_t* pStudioModel = interfaces::model_info->studio_model( e->mdl( ) );
	mstudiohitboxset_t* set = pStudioModel->hitbox_set( 0 );

	if ( !set )
		return false;

	mstudiobbox_t* studio_box = set->hitbox( box );
	if ( !studio_box )
		return false;

	vec3_t min , max;

	const auto is_capsule = studio_box->radius != -1.f;


	if ( is_capsule )
	{
		math::VectorTransform( studio_box->bbmin , record->bones [ studio_box->bone ] , min );
		math::VectorTransform( studio_box->bbmax , record->bones [ studio_box->bone ] , max );
		const auto dist = math::segment_to_segment( start , end , min , max );

		if ( dist < studio_box->radius )
			return true;
	}
	else

	{
		trace_t tr;

		// setup trace data
		e->origin( ) = record->orig;
		e->abs_origin( ) = record->orig;
		e->set_abs_angles( record->abs_ang );
		e->mins( ) = record->mins;
		e->maxs( ) = record->maxs;
		memcpy( e->bone_cache( ) , matrix , sizeof( matrix_t ) * e->bone_count( ) );

		ray_t ray;
		ray.initialize( start , end );
		// setup ray and trace.
		interfaces::trace_ray->clip_ray_to_entity( ray , mask_shot , e , &tr );

		// restore trace data
		record->orig = backup_origin;
		record->abs_orig = backup_abs_origin;
		record->abs_ang = backup_abs_angles;
		record->mins = backup_obb_mins;
		record->maxs = backup_obb_maxs;
		memcpy( e->bone_cache( ) , backup_cache , sizeof( matrix_t ) * e->bone_count( ) );

		// check if we hit a valid player / hitgroup on the player and increment total hits.
		if ( tr.entity == e && tr.hitbox == box )
			return true;
	}

	return false;
}
void get_hitbox_data( C_Hitbox* rtn , base_entity* ent , int ihitbox , matrix_t* matrix )
{
	if ( ihitbox < 0 || ihitbox > 19 ) return;

	if ( !ent ) return;

	const auto model = ent->mdl( );

	if ( !model )
		return;

	auto pStudioHdr = interfaces::model_info->studio_model( model );
	mstudiohitboxset_t* set = pStudioHdr->hitbox_set( 0 );
	if ( !set )
		return;

	mstudiobbox_t* hitbox = set->hitbox( ihitbox );
	if ( !hitbox )
		return;

	const auto is_capsule = hitbox->radius != -1.f;

	vec3_t min , max;
	if ( is_capsule ) {
		math::VectorTransform( hitbox->bbmin , matrix [ hitbox->bone ] , min );
		math::VectorTransform( hitbox->bbmax , matrix [ hitbox->bone ] , max );
	}
	else
	{
		math::VectorTransform( math::vector_rotate( hitbox->bbmin , hitbox->angle ) , matrix [ hitbox->bone ] , min );
		math::VectorTransform( math::vector_rotate( hitbox->bbmax , hitbox->angle ) , matrix [ hitbox->bone ] , max );
	}

	rtn->hitboxID = ihitbox;
	rtn->isOBB = !is_capsule;
	rtn->radius = hitbox->radius;
	rtn->mins = min;
	rtn->maxs = max;
	rtn->bone = hitbox->bone;
}
auto transform = [ ] ( const vec3_t& in1 , const matrix_t& in2 , vec3_t& out )
{
	auto VectorTransform = [ ] ( const float* in1 , const matrix_t& in2 , float* out )
	{
		auto DotProducts = [ ] ( const float* v1 , const float* v2 )
		{
			return v1 [ 0 ] * v2 [ 0 ] + v1 [ 1 ] * v2 [ 1 ] + v1 [ 2 ] * v2 [ 2 ];
		};
		out [ 0 ] = DotProducts( in1 , in2 [ 0 ] ) + in2 [ 0 ][ 3 ];
		out [ 1 ] = DotProducts( in1 , in2 [ 1 ] ) + in2 [ 1 ][ 3 ];
		out [ 2 ] = DotProducts( in1 , in2 [ 2 ] ) + in2 [ 2 ][ 3 ];
	};
	VectorTransform( &in1.x , in2 , &out.x );
};
enum {
	crosshair ,
	health ,
	distance
};

bool is_visible( base_entity* pl , int hitbox ) {
	if ( !pl || !pl->is_alive( ) )
		return false;
	trace_t tr;
	trace_filter traceFilter;
	ray_t ray;

	ray.initialize( g::local->eye_pos( ) , pl->get_hitbox_pos( hitbox ) );
	if ( g::local )
	{
		traceFilter.skip = g::local;
		interfaces::trace_ray->trace_ray( ray , mask_visible , &traceFilter , &tr );
		if ( tr.entity == pl || tr.flFraction >= 0.6f )
			return true;
	}

	return false;
}
std::vector<vec3_t> multipoint( base_entity* e , records* record , int hb , c_weapon* wpn )
{
	std::vector<vec3_t> _final;
	studiohdr_t* pStudioModel = interfaces::model_info->studio_model( e->mdl( ) );
	mstudiohitboxset_t* set = pStudioModel->hitbox_set( 0 );
	mstudiobbox_t* hitbox = set->hitbox( hb );

	auto center = ( hitbox->bbmin + hitbox->bbmax ) * 0.5f;
	vec3_t point;

	vec3_t left;
	vec3_t right;

	vec3_t top;
	vec3_t nrml;
	transform( center , record->bones [ hitbox->bone ] , nrml );
	auto v7 = wpn->spread( ) + wpn->inaccuracy( );
	auto pDist = nrml.dist_to( g::local->eye_pos( ) ) / std::sin( deg2rad( 90.f - rad2deg( v7 ) ) );
	auto xrad = hitbox->radius - pDist * v7;
	std::clamp( xrad , 0.f , hitbox->radius * 0.75f );
	auto i = xrad;

	switch ( hb )
	{
	case head:
		if ( is_visible( e , head ) )
		{
			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.75f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( top );
			for ( float a = 0; a < twopi; a += stp ) {
				vec3_t start( hitbox->radius * cosf( a ) + nrml.x , hitbox->radius * sinf( a ) + nrml.y , nrml.z );
				vec3_t end( hitbox->radius * cosf( a + stp ) + nrml.x , hitbox->radius * sinf( a + stp ) + nrml.y , nrml.z );
				_final.push_back( start );
				_final.push_back( end );
			}
		}
		else
			_final.push_back( nrml );
		break;
	case chest:
		if ( is_visible( e , chest ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case upper_chest:
		if ( is_visible( e , upper_chest ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case stomach:
		if ( is_visible( e , stomach ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case pelvis:
		if ( is_visible( e , pelvis ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case left_thigh:
		if ( is_visible( e , left_thigh ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case right_thigh:
		if ( is_visible( e , right_thigh ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case left_foot:
		if ( is_visible( e , left_foot ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;
	case right_foot:
		if ( is_visible( e , right_foot ) )
		{
			point = center + vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , left );

			point = center - vec3_t( 0 , 0 , 1 ) * i;
			transform( point , record->bones [ hitbox->bone ] , right );

			point = center - vec3_t( 0 , 1 , 0 ) * hitbox->radius * 0.2f;
			transform( point , record->bones [ hitbox->bone ] , top );

			_final.push_back( left );
			_final.push_back( right );
			_final.push_back( top );
		}
		else
			_final.push_back( nrml );
		break;

	default:
		_final.push_back( nrml );
		break;
	}

	return _final;

	//upper chest
	//interfaces::debug_overlay->box_overlay( center + Right * hitbox->radius * .5f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
	//chest
	/*interfaces::debug_overlay->box_overlay( center + Left * hitbox->radius * 1.5f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
	interfaces::debug_overlay->box_overlay( center + Right * hitbox->radius * .5f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );*/

	//stomach/pelvis

//cap
/*interfaces::debug_overlay->box_overlay( center + Left * hitbox->radius * .8f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
interfaces::debug_overlay->box_overlay( center + Top * hitbox->radius * 1.4f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
interfaces::debug_overlay->box_overlay( center + Right * hitbox->radius * .8f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
interfaces::debug_overlay->box_overlay( center + Right_elevated * hitbox->radius * .8f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );
interfaces::debug_overlay->box_overlay( center + Left_elevated * hitbox->radius * .8f , vec3_t( -.3 , -.3 , -.3 ) , vec3_t( .3 , .3 , .3 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 127 , 0.1f );*/

}

bool safepoint( base_entity* pl , features::lagcomp::records* rec , vec3_t eye_pos , c_weapon* wpn , int box )
{
	if ( !g_var.ragebot.safepoint_type )
		return true;

	if ( !rec )
		return false;
	if ( !pl || !pl->is_alive( ) )
		return false;

	vec3_t angle;
	for ( auto i : multipoint( pl , rec , box , wpn ) )
	{
		angle = math::calculate_angle( eye_pos , i );
	}

	vec3_t forward;
	math::angle_2vectors( angle , &forward );

	auto end = eye_pos + forward * 8092.f;

	C_Hitbox box1; get_hitbox_data( &box1 , pl , box , rec->left );
	C_Hitbox box2; get_hitbox_data( &box2 , pl , box , rec->right );
	C_Hitbox box3; get_hitbox_data( &box3 , pl , box , rec->bones );

	auto available = box1.isOBB || box2.isOBB || box3.isOBB;
	if ( available )
	{
		math::vector_i_transform( eye_pos , rec->left [ box1.bone ] , box1.mins );
		math::vector_i_rotate( end , rec->left [ box1.bone ] , box1.maxs );

		math::vector_i_transform( eye_pos , rec->right [ box2.bone ] , box2.mins );
		math::vector_i_rotate( end , rec->right [ box2.bone ] , box2.maxs );

		math::vector_i_transform( eye_pos , rec->bones [ box3.bone ] , box3.mins );
		math::vector_i_rotate( end , rec->bones [ box3.bone ] , box3.maxs );

	}

	auto hits = 0;

	if ( available ? math::intersect_line_with_bb( eye_pos , end , box1.mins , box1.maxs ) : math::intersect( eye_pos , end , box1.mins , box1.maxs , box1.radius ) )
		++hits;

	if ( available ? math::intersect_line_with_bb( eye_pos , end , box2.mins , box2.maxs ) : math::intersect( eye_pos , end , box2.mins , box2.maxs , box2.radius ) )
		++hits;

	if ( available ? math::intersect_line_with_bb( eye_pos , end , box3.mins , box3.maxs ) : math::intersect( eye_pos , end , box3.mins , box3.maxs , box3.radius ) )
		++hits;

	return( hits >= 3 );
}

namespace features
{
	namespace ragebot
	{
		void backup_rec( base_entity* pl , records* rec )
		{
			auto i = pl->idx( );
			backup_anims [ i ].orig = pl->origin( );
			backup_anims [ i ].abs_orig = pl->abs_origin( );
			backup_anims [ i ].abs_ang = pl->abs_angles( );
			backup_anims [ i ].mins = pl->mins( );
			backup_anims [ i ].maxs = pl->maxs( );
			memcpy( backup_anims [ i ].bones , pl->bone_cache( ) , sizeof( matrix_t ) * pl->bone_count( ) );
		}
		void set_anims( base_entity* pl , records* rec )
		{
			pl->inval_bone_cache( );
			pl->mins( ) = rec->mins;
			pl->maxs( ) = rec->maxs;
			pl->origin( ) = rec->orig;
			pl->set_abs_origin( rec->abs_orig );
			pl->set_abs_angles( rec->abs_ang );
			memcpy( pl->bone_cache( ) , rec->bones , sizeof( matrix_t ) * pl->bone_count( ) );

		}
		void restore_anims( base_entity* pl , records* rec )
		{
			auto i = pl->idx( );
			pl->mins( ) = backup_anims [ i ].mins;
			pl->maxs( ) = backup_anims [ i ].maxs;
			pl->origin( ) = backup_anims [ i ].orig;
			pl->set_abs_origin( backup_anims [ i ].abs_orig );
			pl->set_abs_angles( backup_anims [ i ].abs_ang );
			memcpy( pl->bone_cache( ) , backup_anims [ i ].bones , sizeof( matrix_t ) * pl->bone_count( ) );
		}


		bool hitchance( vec3_t angles , records* record , base_entity* e , int chance , int hitbox )
		{
			auto weapon = g::local->weapon( );

			if ( !e || !weapon || !record )
				return false;

			const auto studio_model = interfaces::model_info->studio_model( e->mdl( ) );

			if ( !studio_model )
				return false;

			vec3_t forward , right , up;
			vec3_t src = g::local->eye_pos( );
			math::angle_vec( angles , forward , right , up );

			int cHits = 0;
			int cNeededHits = static_cast< int >( 150.f * ( chance / 100.f ) );

			float weap_spread = weapon->spread( );
			float weap_inaccuracy = weapon->inaccuracy( );

			for ( int i = 0; i < 150; i++ )
			{
				float a = math::random_float( 0.f , 1.f );
				float b = math::random_float( 0.f , 2.f * pi_f );
				float c = math::random_float( 0.f , 1.f );
				float d = math::random_float( 0.f , 2.f * pi_f );

				float inaccuracy = a * weap_inaccuracy;
				float spread = c * weap_spread;


				if ( weapon->item_definition_index( ) == 64 )
				{
					a = 1.f - a * a;
					a = 1.f - c * c;
				}

				vec3_t spreadView( ( cos( b ) * inaccuracy ) + ( cos( d ) * spread ) , ( sin( b ) * inaccuracy ) + ( sin( d ) * spread ) , 0 ) , direction;

				direction.x = forward.x + ( spreadView.x * right.x ) + ( spreadView.y * up.x );
				direction.y = forward.y + ( spreadView.x * right.y ) + ( spreadView.y * up.y );
				direction.z = forward.z + ( spreadView.x * right.z ) + ( spreadView.y * up.z );
				direction.normalized( );

				vec3_t viewAnglesSpread;
				math::vector_angles3( direction , up , viewAnglesSpread );
				math::normalize_angle( viewAnglesSpread );

				vec3_t viewForward;
				math::angle_2vectors( viewAnglesSpread , &viewForward );
				viewForward.normalize( );

				viewForward = src + ( viewForward * weapon->weapon_data( )->range );

				if ( CanHitHitbox( src , viewForward , record , e , studio_model , hitbox ) )
					++cHits;

				if ( static_cast< int >( ( static_cast< float >( cHits ) / 150.f ) * 100.f ) >= chance )
					return true;

				if ( ( 150 - i + cHits ) < cNeededHits )
					return false;

			}
			return false;
		}

		bool can_shoot( )
		{
			if ( !g::local || !g::local->is_alive( ) )
				return false;

			if ( interfaces::global_vars->curtime <= g::local->m_flNextAttack( ) )
				return false;

			auto wpn = g::local->weapon( );

			if ( !wpn )
				return false;

			static float lastFire = 0 , nextAttack = 0;
			static c_weapon* old_weapon;

			if ( lastFire != wpn->last_shot_time( ) || wpn != old_weapon )
			{
				lastFire = wpn->last_shot_time( );
				nextAttack = wpn->next_primary_attack( );
			}

			if ( wpn->clip1_count( ) == 0 )
				return false;

			old_weapon = wpn;

			return ( nextAttack <= ( float ) ( g::local->tick_base( ) - ( exploit::tickbase_shift > 0 ? 1 + exploit::tickbase_shift : 0 ) ) * interfaces::global_vars->interval_per_tick );
		}

		auto get_predicted_damage = [ ] ( base_entity* target , vec3_t point ) {
			vec3_t last_shoot_pos = g::eye_pos;
			g::eye_pos = g::local->pred_eyepos( 2 );
			float ret = -1.f;

			autowall::penetration_output_t out;
			if ( autowall::run( target , point , 1 , &out ) ) ret = out.m_damage;
			g::eye_pos = last_shoot_pos;
			return ret;
		};
		vec3_t scan( records* record , base_entity* e , c_weapon* wpn , int& hb )
		{
			if ( !e || !e->is_alive( ) )
				return vec3_t( );

			if ( !wpn )
				return vec3_t( );
			int min_dmg = g_var.ragebot.min_dmg;
			if ( g_var.ragebot.dmg_override && keyhandler::auto_check( g_var.ragebot.dmg_bind [ 1 ] , g_var.ragebot.dmg_bind [ 0 ] ) )
				min_dmg = g_var.ragebot.override_dmg;
			vec3_t bestpoint;
			set_anims( e , record );

			for ( auto x : hitboxes( e , wpn ) )
			{
				for ( auto point : multipoint( e , record , x , wpn ) )
				{
					autowall::penetration_output_t pen_output;

					if ( get_predicted_damage( e , point ) > 10 )
						pre_stop = true;

					if ( autowall::run( e , point , 1 , &pen_output ) ) {
						if ( pen_output.m_damage >= min_dmg || pen_output.m_damage >= e->health( ) )
						{
							hb = x;
							min_dmg = pen_output.m_damage;
							bestpoint = point;
						}
					}
				}
			}
			restore_anims( e , record );
			return bestpoint;
		}

		void handle_misses( )
		{
			if ( shot_snapshots.size( ) == 0 )
				return;
			auto& snapshot = shot_snapshots.front( );
			if ( fabs( ( interfaces::global_vars->curtime ) - snapshot.time ) > 1.f )
			{
				shot_snapshots.clear( );
				return;
			}

			if ( snapshot.first_processed_time != -1.f ) {
				if ( snapshot.damage == -1 && snapshot.weapon_fire && snapshot.bullet_impact && snapshot.entity ) {
					player_info_t info;
					interfaces::engine->get_player_info( snapshot.entity->idx( ) , &info );
					std::string msg;
					msg += "missed shot at " + std::string( info.name ) + " due to";
					bool spread = false;
					if ( snapshot.entity ) {
						const auto studio_model = interfaces::model_info->studio_model( snapshot.entity->mdl( ) );

						if ( studio_model )
						{
							auto angle = math::calculate_angle( snapshot.start , snapshot.impact );
							vec3_t forward;
							math::angle_2vectors( angle , &forward );
							auto end = snapshot.impact + forward * 2000.f;
							if ( !CanHitHitbox( snapshot.start , end , snapshot.record , snapshot.entity , studio_model , snapshot.hitbox ) )
								spread = true;
						}
					}
					events::event_t csgo_event;
					std::stringstream szMessage;
					color msg_col;


					if ( spread )
					{
						msg += " spread.";
						msg_col = color( 255 , 255 , 0 );
					}
					else if ( !spread && g_var.ragebot.resolver )
					{
						misses [ snapshot.entity->idx( ) ]++;
						msg += " desync.";
						msg_col = color( 255 , 0 , 0 , 255 );
					}
					else
					{
						misses [ snapshot.entity->idx( ) ]++;
						msg += " spread.";
						msg_col = color( 255 , 0 , 0 , 255 );
					}
					msg += "[ T: " + std::to_string( snapshot.backtrack ) + " BT: " + ( snapshot.backtrack ? "Y ]" : "N ]" );
					csgo_event.clr = msg_col;
					csgo_event.erase1 = interfaces::global_vars->curtime + 2.f;
					csgo_event.erase2 = interfaces::global_vars->curtime + 2.4f;
					interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[moonlight] " );
					szMessage << msg;
					csgo_event.msg = szMessage.str( );
					szMessage << " \n";
					interfaces::cvar->console_color_printf( c_color( msg_col.r , msg_col.g , msg_col.b ) , szMessage.str( ).c_str( ) );

					events::events.EventList.push_back( csgo_event );

					shot_snapshots.erase( shot_snapshots.begin( ) );
				}
			}
		}

		void init( )
		{
			data.clear( );
			ang = vec3_t( );
			hb = -1;
			b_rec = nullptr;
		}

		void run( usercmd* cmd )
		{
			set_config( );
			if ( !g_var.ragebot.enable )
				return;

			if ( !g::local || !g::local->is_alive( ) )
				return;
			auto wpn = g::local->weapon( );
			if ( !wpn || wpn->is_knife( ) || wpn->is_bomb( ) || wpn->is_grenade( ) || wpn->clip1_count( ) <= 0 )
			{
				ragebot::pre_stop = false;
				return;
			}
			if ( !can_shoot( ) ) { cmd->buttons &= ~in_attack; ragebot::pre_stop = false; return; }
			auto sorter = [ ] ( target first , target second ) -> bool {
				vec3_t viewangles;
				viewangles = interfaces::engine->get_view_angles( );

				switch ( g_var.target_selection ) {
				case 0: return math::get_fov( viewangles , first.e->eye_pos( ) ) < math::get_fov( viewangles , second.e->eye_pos( ) ); break;
				case 1: return first.e->health( ) < second.e->health( ); break;
				case 2: return ( first.e->origin( ) - g::local->eye_pos( ) ).length( ) < ( second.e->origin( ) - g::local->eye_pos( ) ).length( ); break;
				}
				return false;
			};

			for ( int i = 1; i <= 64; i++ ) {
				auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !e || !e->is_alive( ) || e == g::local || e->team( ) == g::local->team( ) || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
				{
					ragebot::pre_stop = false;
					continue;
				}

				auto info = target( e );

				data.push_back( info );

				if ( data.size( ) >= 4 ) {
					auto first = rand( ) % data.size( );
					auto second = rand( ) % data.size( );
					auto third = rand( ) % data.size( );

					for ( auto i = 0; i < data.size( ); ++i ) {
						if ( i == first || i == second || i == third )
							continue;

						data.erase( data.begin( ) + i );

						if ( i > 0 )
							--i;
					}

				}
				std::sort( data.begin( ) , data.end( ) , sorter );
			}

			/* sort targets by our values */

			for ( auto it : data )
			{

				auto e = it.e;
				if ( !e || !e->is_alive( ) || e == g::local || e->team( ) == g::local->team( ) || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
					continue;

				records o_rec( e );
				backup_rec( e , &o_rec );


				auto newest = get_records( e , false );

				if ( newest )
				{
					vec3_t angle = scan( newest , e , wpn , hb );
					if ( angle != vec3_t( 0 , 0 , 0 ) )
					{
						ang = angle;
						b_rec = newest;
						ent = e;
						continue;
					}
				}
				auto last = get_records( e , true );
				if ( last )
				{
					vec3_t angle = scan( last , e , wpn , hb );
					if ( angle != vec3_t( 0 , 0 , 0 ) )
					{
						ang = angle;
						b_rec = last;
						ent = e;
						continue;
					}
				}
			}

			if ( b_rec && ang != vec3_t( ) && hb != -1 && ent && ent->is_alive( ) )
			{
				vec3_t aimpoint = math::normalize_angle( math::calculate_angle( g::local->eye_pos( ) , ang ) );
				if ( aimpoint != vec3_t( ) )
				{
					can_stop = true;
					if ( g_var.ragebot.refine_shot )
						if ( b_rec->choked > 12 )//unfinished
							return;

					bool hc = hitchance( aimpoint , b_rec , ent , g_var.ragebot.hitchance , hb );


					if ( g_var.ragebot.autoscope && !hc )
						if ( wpn->is_scopeable( ) )
							if ( wpn->zoom( ) == 0 )
								cmd->buttons |= in_attack2;

					if ( hc )
					{
						if ( g_var.ragebot.autofire )
							cmd->buttons |= in_attack;
					}

					if ( g_var.ragebot.exploit == 1 && exploit::shot_cmdnr + 15 >= g::cmd->command_number && !keyhandler::auto_check( g_var.antihit.fakeduck_bind [ 1 ] , g_var.antihit.fakeduck_bind [ 0 ] ) )
					{
						can_stop = true;
						cmd->buttons |= in_attack;
					}

					if ( cmd->buttons & in_attack )
					{
						wpn->update_accuracy( );
						ShotSnapshot snapshot;
						snapshot.entity = ent;
						snapshot.time = interfaces::global_vars->curtime;
						snapshot.first_processed_time = 0.f;
						snapshot.bullet_impact = false;
						snapshot.weapon_fire = false;
						snapshot.damage = -1;
						snapshot.hitbox = hb;
						snapshot.start = g::local->eye_pos( );
						snapshot.eyeangles = math::normalize_angle( ent->angles( ) );
						snapshot.record = b_rec;
						snapshot.backtrack = time_to_ticks( fabsf( b_rec->simtime - ent->simtime( ) ) );
						shot_snapshots.push_back( snapshot );

						g::sendpacket = !keyhandler::auto_check( g_var.antihit.fakeduck_bind [ 1 ] , g_var.antihit.fakeduck_bind [ 0 ] );

						aimpoint -= g::local->aim_punch_angle( ) * interfaces::cvar->find_var( "weapon_recoil_scale" )->get_float( );
						cmd->tick_count = time_to_ticks( b_rec->simtime + lerp_time( ) );
						cmd->viewangles = aimpoint;
					}
				}
			}
		}
	}
}
