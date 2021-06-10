#pragma once

#include <sdk/math/vec3.h>
#include <minwindef.h>
#include <windows.h>
class base_entity;
#define	surf_light		0x0001		// value will hold the light strength
#define	surf_sky2d		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3d skybox
#define	surf_sky		0x0004		// don't draw, but add to skybox
#define	surf_warp		0x0008		// turbulent water warp
#define	surf_trans		0x0010
#define surf_noportal	0x0020	// the surface can not have a portal placed on it
#define	surf_trigger	0x0040	// fixme: this is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	surf_nodraw		0x0080	// don't bother referencing the texture
#define	surf_hint		0x0100	// make a primary bsp splitter
#define	surf_skip		0x0200	// completely ignore, allowing non-closed brushes
#define surf_nolight	0x0400	// don't calculate light
#define surf_bumplight	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define surf_noshadows	0x1000	// don't receive shadows
#define surf_nodecals	0x2000	// don't receive decals
#define surf_nopaint	0x2000	// the surface can not have paint placed on it
#define surf_nochop		0x4000	// don't subdivide patches on this surface
#define surf_hitbox		0x8000	// surface is part of a hitbox

#define char_tex_antlion		'a'
#define char_tex_bloodyflesh	'b'
#define	char_tex_concrete		'c'
#define char_tex_dirt			'd'
#define char_tex_eggshell		'e' ///< the egg sacs in the tunnels in ep2.
#define char_tex_flesh			'f'
#define char_tex_grate			'g'
#define char_tex_alienflesh		'h'
#define char_tex_clip			'i'
//#define char_tex_unused		'j'
#define char_tex_snow			'k'
#define char_tex_plastic		'l'
#define char_tex_metal			'm'
#define char_tex_sand			'n'
#define char_tex_foliage		'o'
#define char_tex_computer		'p'
//#define char_tex_unused		'q'
#define char_tex_reflective		'r'
#define char_tex_slosh			's'
#define char_tex_tile			't'
#define char_tex_cardboard		'u'
#define char_tex_vent			'v'
#define char_tex_wood			'w'
//#define char_tex_unused		'x' ///< do not use - "fake" materials use this (ladders, wading, clips, etc)
#define char_tex_glass			'y'
#define char_tex_warpshield		'z' ///< wierd-looking jello effect for advisor shield.

#define	contents_empty			0		/**< no contents. */
#define	contents_solid			0x1		/**< an eye is never valid in a solid . */
#define	contents_window			0x2		/**< translucent, but not watery (glass). */
#define	contents_aux			0x4
#define	contents_grate			0x8		/**< alpha-tested "grate" textures.  bullets/sight pass through, but solids don't. */
#define	contents_slime			0x10
#define	contents_water			0x20
#define	contents_mist			0x40
#define contents_opaque			0x80		/**< things that cannot be seen through (may be non-solid though). */
#define	last_visible_contents	0x80
#define all_visible_contents 	(last_visible_contents | (last_visible_contents-1))
#define contents_testfogvolume	0x100
#define contents_unused5		0x200
#define contents_unused6		0x4000
#define contents_team1			0x800		/**< per team contents used to differentiate collisions. */
#define contents_team2			0x1000		/**< between players and objects on different teams. */
#define contents_ignore_nodraw_opaque	0x2000		/**< ignore contents_opaque on surfaces that have surf_nodraw. */
#define contents_moveable		0x4000		/**< hits entities which are movetype_push (doors, plats, etc) */
#define	contents_areaportal		0x8000		/**< remaining contents are non-visible, and don't eat brushes. */
#define	contents_playerclip		0x10000
#define	contents_monsterclip	0x20000

/**
* @section currents can be added to any other contents, and may be mixed
*/
#define	contents_current_0		0x40000
#define	contents_current_90		0x80000
#define	contents_current_180	0x100000
#define	contents_current_270	0x200000
#define	contents_current_up		0x400000
#define	contents_current_down	0x800000

/**
* @endsection
*/

#define	contents_origin			0x1000000	/**< removed before bsping an entity. */
#define	contents_monster		0x2000000	/**< should never be on a brush, only in game. */
#define	contents_debris			0x4000000
#define	contents_detail			0x8000000	/**< brushes to be added after vis leafs. */
#define	contents_translucent	0x10000000	/**< auto set if any surface has trans. */
#define	contents_ladder			0x20000000
#define contents_hitbox			0x40000000	/**< use accurate hitboxes on trace. */

/**
* @section trace masks.
*/
#define	mask_all				(0xffffffff)
#define	mask_solid				(contents_solid|contents_moveable|contents_window|contents_monster|contents_grate) 			/**< everything that is normally solid */
#define	mask_playersolid		(contents_solid|contents_moveable|contents_playerclip|contents_window|contents_monster|contents_grate) 	/**< everything that blocks player movement */
#define	mask_npcsolid			(contents_solid|contents_moveable|contents_monsterclip|contents_window|contents_monster|contents_grate) /**< blocks npc movement */
#define	mask_water				(contents_water|contents_moveable|contents_slime) 							/**< water physics in these contents */
#define	mask_opaque				(contents_solid|contents_moveable|contents_opaque) 							/**< everything that blocks line of sight for ai, lighting, etc */
#define mask_opaque_and_npcs	(mask_opaque|contents_monster)										/**< everything that blocks line of sight for ai, lighting, etc, but with monsters added. */
#define	mask_visible			(mask_opaque|contents_ignore_nodraw_opaque) 								/**< everything that blocks line of sight for players */
#define mask_visible_and_npcs	(mask_opaque_and_npcs|contents_ignore_nodraw_opaque) 							/**< everything that blocks line of sight for players, but with monsters added. */
#define	mask_shot				(contents_solid|contents_moveable|contents_monster|contents_window|contents_debris|contents_hitbox) 	/**< bullets see these as solid */
#define mask_shot_hull			(contents_solid|contents_moveable|contents_monster|contents_window|contents_debris|contents_grate) 	/**< non-raycasted weapons see this as solid (includes grates) */
#define mask_shot_portal		(contents_solid|contents_moveable|contents_window) 							/**< hits solids (not grates) and passes through everything else */
#define mask_shot_brushonly			(contents_solid|contents_moveable|contents_window|contents_debris) // non-raycasted weapons see this as solid (includes grates)
#define mask_solid_brushonly	(contents_solid|contents_moveable|contents_window|contents_grate) 					/**< everything normally solid, except monsters (world+brush only) */
#define mask_playersolid_brushonly	(contents_solid|contents_moveable|contents_window|contents_playerclip|contents_grate) 			/**< everything normally solid for player movement, except monsters (world+brush only) */
#define mask_npcsolid_brushonly	(contents_solid|contents_moveable|contents_window|contents_monsterclip|contents_grate) 			/**< everything normally solid for npc movement, except monsters (world+brush only) */
#define mask_npcworldstatic		(contents_solid|contents_window|contents_monsterclip|contents_grate) 					/**< just the world, used for route rebuilding */
#define mask_splitareaportal	(contents_water|contents_slime) 									/**< These are things that can split areaportals */

class __declspec( align( 16 ) )vec3_tAligned : public vec3_t {
public:
	vec3_tAligned& operator=( const vec3_t& vOther ) {
		init( vOther.x , vOther.y , vOther.z );
		return *this;
	}

	float w;
};

class IHandleEntity;

struct ray_t {
	vec3_tAligned m_start; // starting point, centered within the extents
	vec3_tAligned m_delta; // direction + length of the ray
	vec3_tAligned m_start_offset; // Add this to m_Start to get the actual ray start
	vec3_tAligned m_extents; // Describes an axis aligned box extruded along a ray
	const matrix_t* m_world_axis_transform;
	//const matrix_t *m_pWorldAxisTransform;
	bool m_is_ray; // are the extents zero?
	bool m_is_swept; // is delta != 0?

	void initialize( const vec3_t& start , const vec3_t& end ) {
		m_delta = end - start;

		m_is_swept = ( m_delta.length_sqr( ) != 0 );

		m_extents.x = m_extents.y = m_extents.z = 0.0f;
		m_is_ray = true;

		m_start_offset.x = m_start_offset.y = m_start_offset.z = 0.0f;

		m_start = start;
	}

	void initialize( vec3_t& vecStart , vec3_t& vecEnd , vec3_t min , vec3_t max ) {
		m_delta = vecEnd - vecStart;

		m_is_swept = ( m_delta.length_sqr( ) != 0 );

		m_extents.x = ( max.x - min.x );
		m_extents.y = ( max.y - min.y );
		m_extents.z = ( max.z - min.z );
		m_is_ray = false;

		m_start_offset.x = m_start_offset.y = m_start_offset.z = 0.0f;

		m_start = vecStart + ( ( max + min ) * 0.5f );
	}

private:
};

struct csurface_t {
	const char* name;
	short surfaceProps;
	unsigned short flags;
};

struct surfacephysicsparams_t
{
	float    friction;
	float    elasticity;
	float    density;
	float    thickness;
	float    dampening;
};
struct surfaceaudioparams_t
{
	float    reflectivity;             // like elasticity, but how much sound should be reflected by this surface
	float    hardnessFactor;           // like elasticity, but only affects impact sound choices
	float    roughnessFactor;          // like friction, but only affects scrape sound choices   
	float    roughThreshold;           // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float    hardThreshold;            // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float    hardVelocityThreshold;    // collision velocity > this causes "hard" impacts, < this causes "soft" impacts   
	float    highPitchOcclusion;       //a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float    midPitchOcclusion;
	float    lowPitchOcclusion;
};
struct surfacesoundnames_t
{
	unsigned short    walkStepLeft;
	unsigned short    walkStepRight;
	unsigned short	  runStepLeft;
	unsigned short	  runStepRight;
	unsigned short    impactSoft;
	unsigned short    impactHard;
	unsigned short    scrapeSmooth;
	unsigned short    scrapeRough;
	unsigned short    bulletImpact;
	unsigned short    rolling;
	unsigned short    breakSound;
	unsigned short    strainSound;
};
struct surfacegameprops_t
{
public:
	float maxSpeedFactor;
	float jumpFactor;
	float flPenetrationModifier;
	float flDamageModifier;
	unsigned short material;
	byte climbable;
	char pad00[ 0x4 ];

};
struct surfacedata_t
{
	surfacephysicsparams_t    physics;
	surfaceaudioparams_t    audio;
	surfacesoundnames_t        sounds;
	surfacegameprops_t        game;
};

class physic_props
{
public:
	int	GetSurfaceIndex( const char* pSurfacePropName )
	{
		using original_fn = int ( __thiscall* )( void* , const char* );
		return ( *( original_fn** ) this )[ 4 ]( this , pSurfacePropName );
	}
	surfacedata_t* get_surface_data( int surfaceDataIndex )
	{
		using original_fn = surfacedata_t * ( __thiscall* )( void* , int );
		return ( *( original_fn** ) this )[ 5 ]( this , surfaceDataIndex );
	}
};


using should_hit_func_t = bool( __cdecl* )( base_entity* , int );

class trace_filter_simple_game_t {
public:
	void* m_vmt;
	const base_entity* m_pass_ent1;
	int m_collision_group;
	should_hit_func_t m_shouldhit_check_fn;

public:
	trace_filter_simple_game_t( );
	trace_filter_simple_game_t( const base_entity* pass_ent1 , int collision_group = 0 , should_hit_func_t shouldhit_check_fn = nullptr );

	bool should_hit_ent( base_entity* entity , int contents_mask ) {
		void* real_vmt = *( void** ) m_vmt;

		return vfunc< bool( __thiscall* )( void* , base_entity* , int ) >( real_vmt , 0 )( real_vmt , entity , contents_mask );
	}

	void set_pass_entity( base_entity* pass_ent1 ) {
		m_pass_ent1 = pass_ent1;
	}

	void set_collision_group( int collision_group ) {
		m_collision_group = collision_group;
	}
};

class trace_filter_skip_two_entities_game_t {
public:
	void* m_vmt;
	const base_entity* m_pass_ent1;
	int m_collision_group;
	should_hit_func_t m_shouldhit_check_fn;
	const base_entity* m_pass_ent2;

public:
	trace_filter_skip_two_entities_game_t( );
	trace_filter_skip_two_entities_game_t( const base_entity* pass_ent1 , const base_entity* pass_ent2 , int collision_group = 0 , should_hit_func_t shouldhit_check_fn = nullptr );

	bool should_hit_ent( base_entity* entity , int contents_mask ) {
		void* real_vmt = *( void** ) m_vmt;

		return vfunc< bool( __thiscall* )( void* , base_entity* , int ) >( real_vmt , 0 )( real_vmt , entity , contents_mask );
	}

	void set_pass_entity( base_entity* pass_ent1 ) {
		m_pass_ent1 = pass_ent1;
	}

	void set_collision_group( int collision_group ) {
		m_collision_group = collision_group;
	}

	void set_pass_entity2( base_entity* pass_ent2 ) {
		m_pass_ent2 = pass_ent2;
	}
};
struct cplane_t {
	vec3_t normal;
	float m_dist;
	BYTE m_type;
	BYTE m_sign_bits;
	BYTE m_pad[ 2 ];
};

struct trace_t {
	bool did_hit( ) const {
		return flFraction < 1.f;
	}

	vec3_t start;
	vec3_t end;
	cplane_t plane;
	float flFraction;
	int contents;
	unsigned short dispFlags;
	bool allsolid;
	bool startSolid;
	float fractionLeftSolid;
	csurface_t surface;
	int hitGroup;
	short physicsBone;
	base_entity* entity;
	int hitbox;

	bool DidHitWorld( ) const {
		return false;
	}

	bool DidHitNonWorldEntity( ) const {
		return entity != NULL && !DidHitWorld( );
	}
};

enum TraceType_t {
	TRACE_EVERYTHING = 0 ,
	TRACE_WORLD_ONLY , // NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY , // NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS , // NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

class i_trace_filter {
public:
	virtual bool ShouldHitEntity( void* pEntity , int contentsMask ) = 0;
	virtual TraceType_t GetTraceType( ) const = 0;
};

class trace_filter : public i_trace_filter {
public:
	bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return ( pEntityHandle != skip );
	}

	TraceType_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* skip;
};

class trace_filter_one_entity : public i_trace_filter {
public:
	bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return ( pEntityHandle == pEntity );
	}

	TraceType_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* pEntity;
};

class trace_filter_one_entity2 : public i_trace_filter {
public:
	bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return ( pEntityHandle == pEntity );
	}

	TraceType_t GetTraceType( ) const {
		return TRACE_ENTITIES_ONLY;
	}

	void* pEntity;
};

class trace_filter_skip_two_entities : public i_trace_filter {
public:
	trace_filter_skip_two_entities( void* pPassEnt1 , void* pPassEnt2 ) {
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return !( pEntityHandle == passentity1 || pEntityHandle == passentity2 );
	}

	virtual TraceType_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};

class trace_filter_skip_one_entity : public i_trace_filter {
public:
	trace_filter_skip_one_entity( void* pPassEnt1 ) {
		passentity1 = pPassEnt1;
	}

	virtual bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return !( pEntityHandle == passentity1 );
	}

	virtual TraceType_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* passentity1;
};

class trace_entity : public i_trace_filter {
public:
	bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return !( pEntityHandle == pSkip1 );
	}

	TraceType_t GetTraceType( ) const {
		return TRACE_ENTITIES_ONLY;
	}

	void* pSkip1;
};

class trace_world_only : public i_trace_filter {
public:
	bool ShouldHitEntity( void* pEntityHandle , int contentsMask ) {
		return false;
	}

	TraceType_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* pSkip1;
};

class collideable_t;
class trace {
public:
	virtual int get_point_contents( const vec3_t& pos , int mask = mask_all ,
		IHandleEntity** ent = nullptr ) = 0;
	virtual int get_point_contents_world( const vec3_t& pos , int mask = mask_all ) = 0;
	virtual int get_point_contents_collideable( collideable_t* collide , const vec3_t& pos ) = 0;
	virtual void clip_ray_to_entity( const ray_t& ray , unsigned int mask , base_entity* ent , trace_t* trace ) = 0;
	virtual void clip_ray_to_collideable( const ray_t& ray , unsigned int mask , collideable_t* collide , trace_t* trace ) =
		0;
	virtual void trace_ray( const ray_t& ray , unsigned int mask , i_trace_filter* filter , trace_t* trace ) = 0;
};