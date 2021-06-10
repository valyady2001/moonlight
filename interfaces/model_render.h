#pragma once
#include <sdk/math/vec3.h>

struct mdlrender_info_t {
	vec3_t origin;
	vec3_t angles;
	PAD( 4 );
	void* renderable;
	void* model;
	matrix_t* model_to_world;
	matrix_t* lighting_offset;
	vec3_t* lighting_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitbox_set;
	std::uint16_t instance;
};

class c_mdlrender {
public:
	virtual int draw_model( int flags , void* renderable , unsigned short instance , int entity_index , void* model , vec3_t const& origin , vec3_t const& angles , int skin , int body , int hitboxset , const matrix_t* modelToWorld = NULL , const matrix_t* pLightingOffset = NULL ) = 0;
	virtual void force_mat( class material_t* mat , int nOverrideType = 0 , int nOverrides = 0 ) = 0;
	virtual bool is_forced_mat( void ) = 0;
	virtual void set_view_target( const class studiohdr_t* studio_hdr , int body_index , const vec3_t& target ) = 0;
	virtual unsigned short create_instance( void* renderable , void* cache = nullptr ) = 0;
	virtual void destroy_instance( unsigned short handle ) = 0;
	virtual void set_static_lighting( unsigned short handle , void* _handle ) = 0;
	virtual void* get_static_lighting( unsigned short handle ) = 0;
	virtual bool change_instance( unsigned short handle , void* renderable ) = 0;
	virtual void add_decal( unsigned short handle , struct ray_t& ray , const vec3_t& decal_ip , int decal_index , int body , bool no_poke_throuh , int max_LOD_to_decal ) = 0;
	virtual void remove_all_decals( unsigned short handle ) = 0;
	virtual bool model_has_decals( unsigned short handle ) = 0;
	virtual void remove_all_decals_from_all_models( ) = 0;
	virtual matrix_t* draw_model_shadow_setup( void* renderable , int body , int skin , void* info , matrix_t* custom_bone_to_world = nullptr ) = 0;
	virtual void draw_model_shadow( void* renderable , void* info , matrix_t* custom_bone_to_world = nullptr ) = 0;
	virtual bool recompute_static_lighting( unsigned short handle ) = 0;
	virtual void release_all_static_prop_color_data( void ) = 0;
	virtual void restore_all_static_prop_color_data( void ) = 0;
	virtual int draw_model_ex( struct mdlrender_info_t& info ) = 0;
	virtual int draw_model_ex_static_prop( struct mdlrender_info_t& info ) = 0;
	virtual bool draw_moel_setup( struct mdlrender_info_t& info , void* state , matrix_t* bone_to_world_out ) = 0;
	virtual void draw_model_execute( void* ctx , void* state , const struct mdlrender_info_t& info , matrix_t* custom_bone_to_world = nullptr ) = 0;
	virtual void setup_lighting( const vec3_t& center ) = 0;
	virtual int draw_static_prop_array_fast( void* props , int count , bool shadow_depth ) = 0;
	virtual void suppress_engine_lighting( bool suppress ) = 0;
	virtual void setup_color_meshes( int total_verts ) = 0;
	virtual void setup_lighting_ex( const vec3_t& center , unsigned short handle ) = 0;
	virtual bool get_brightest_shadowing_light_source( const vec3_t& center , vec3_t& light_pos , vec3_t& light_brightness , bool allow_non_tagged_lights ) = 0;
	virtual void compute_lighting_state( int count , const void* query , void* state , void* env_cubemap_texture ) = 0;
	virtual void get_model_decal_handles( void* decals , int decal_stride , int count , const unsigned short* handles ) = 0;
	virtual void compute_static_lighting_state( int count , const void* query , void* state , void* decal_state , void* static_lightning , void* env_cubemap_texture , void* color_mesh_handles ) = 0;
	virtual void cleanup_static_lighting_state( int count , void* color_mesh_handles ) = 0;
};