#pragma once
#include <sdk/math/vec3.h>
#include <sdk/defines.h>
#include <windows.h>

enum bones_mask
{
	bone_used_mask = 0x0007FF00 ,
	bone_used_by_anything = 0x0007FF00 ,
	bone_used_by_hitbox = 0x00000100 ,    // bone (or child) is used by a hit box
	bone_used_by_attachment = 0x00000200 ,    // bone (or child) is used by an attachment point
	bone_used_by_vertex_mask = 0x0003FC00 ,
	bone_used_by_vertex_lod0 = 0x00000400 ,    // bone (or child) is used by the toplevel model via skinned vertex
	bone_used_by_vertex_lod1 = 0x00000800 ,
	bone_used_by_vertex_lod2 = 0x00001000 ,
	bone_used_by_vertex_lod3 = 0x00002000 ,
	bone_used_by_vertex_lod4 = 0x00004000 ,
	bone_used_by_vertex_lod5 = 0x00008000 ,
	bone_used_by_vertex_lod6 = 0x00010000 ,
	bone_used_by_vertex_lod7 = 0x00020000 ,
	bone_used_by_bone_merge = 0x00040000 ,    // bone is available for bone merge to occur against it

	bone_used_by_server = bone_used_by_hitbox | bone_used_by_vertex_lod0 | bone_used_by_vertex_lod1 | bone_used_by_vertex_lod2
	| bone_used_by_vertex_lod3 | bone_used_by_vertex_lod4 | bone_used_by_vertex_lod5 | bone_used_by_vertex_lod6 | bone_used_by_vertex_lod7
};

struct mstudiobone_t
{
	int sznameindex;
	inline char* const get_name( void ) const {
		return ( ( char* ) this ) + sznameindex;
	}

	int parent;
	int bonecontroller[ 6 ];
	vec3_t pos;
	quaternion quat;
	radian_euler rot;
	vec3_t posscale;
	vec3_t rotscale;

	matrix_t pose_to_bone;
	quaternion alignment;
	int flags;
	int proctype;
	int procindex;
	mutable int physicsbone;

	inline void* get_procedure( ) const {
		if ( procindex == 0 )
			return NULL;
		else
			return  ( void* ) ( ( ( byte* ) this ) + procindex );
	};

	int surfacepropidx;
	inline char* const get_surface_props( void ) const {
		return ( ( char* ) this ) + surfacepropidx;
	}

	inline int get_surface_prop( void ) const {
		return surface_prop_lookup;
	}

	int contents;
	int surface_prop_lookup;
	PAD( 7 );

	mstudiobone_t( ) {}
private:
	mstudiobone_t( const mstudiobone_t& other );
};


struct mstudiobbox_t {
#if 0
	int bone;
	int group;
	vec3_t bbmin;
	vec3_t bbmax;
	int hitbox_name_index;
	PAD( 12 );
	float radius;
	PAD( 16 );

	const char* get_name( ) {
		if ( !this->hitbox_name_index )
			return nullptr;

		return reinterpret_cast< const char* >( reinterpret_cast< std::uint8_t* >( const_cast< mstudiobbox_t* >( this ) ) + this->hitbox_name_index );
	}
#endif

	int     bone;                 // 0x0000
	int     group;                // 0x0004
	vec3_t  bbmin;                // 0x0008
	vec3_t  bbmax;                // 0x0014
	int     hitbox_name_index;	  // 0x0020
	vec3_t  angle;                // 0x0024
	float   radius;               // 0x0030
	PAD( 0x10 );                  // 0x0034

	const char* get_name( ) {
		if ( !this->hitbox_name_index )
			return nullptr;

		return reinterpret_cast< const char* >( reinterpret_cast< std::uint8_t* >( const_cast< mstudiobbox_t* >( this ) ) + this->hitbox_name_index );
	}
};

struct mstudiohitboxset_t {
	int name_index;
	int num_hitboxes;
	int hitbox_index;

	const char* get_name( void ) {
		if ( !this->name_index )
			return nullptr;

		return reinterpret_cast< const char* >( reinterpret_cast< std::uint8_t* >( const_cast< mstudiohitboxset_t* >( this ) ) + this->name_index );
	}

	mstudiobbox_t* hitbox( int i ) const {
		if ( i > this->num_hitboxes )
			return nullptr;

		return reinterpret_cast< mstudiobbox_t* >( reinterpret_cast< std::uint8_t* >( const_cast< mstudiohitboxset_t* >( this ) ) + this->hitbox_index ) + i;
	}
};


struct studiohdr_t {
	int id;
	int version;
	long checksum;
	char name[ 64 ];
	int length;
	vec3_t vec_eye_pos;
	vec3_t vec_illumination_pos;
	vec3_t vec_hull_min;
	vec3_t vec_hull_max;
	vec3_t vec_min;
	vec3_t vec_max;
	int flags;
	int numbones;
	int boneindex;
	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;
	int numlocalanim;
	int localanimindex;
	int numlocalseq;
	int localseqindex;
	int activitylistversion;
	int eventsindexed;
	int numtextures;
	int textureindex;

	inline const char* get_name( void ) const {
		return this->name;
	}

	mstudiohitboxset_t* hitbox_set( int i ) {
		if ( i > this->numhitboxsets )
			return nullptr;

		return reinterpret_cast< mstudiohitboxset_t* > ( reinterpret_cast< std::uint8_t* >( const_cast< studiohdr_t* >( this ) ) + this->hitboxsetindex ) + i;
	}

	mstudiobone_t* bone( int i ) {
		if ( i > this->numbones )
			return nullptr;

		return reinterpret_cast< mstudiobone_t* > ( reinterpret_cast< std::uint8_t* >( const_cast< studiohdr_t* >( this ) ) + this->boneindex ) + i;
	}
};


class c_mdlinfo {
private:

public:
	void* model( int idx ) {
		using getmdl_fn = void* ( __thiscall* )( void* , int );
		return vfunc< getmdl_fn >( this , 1 )( this , idx );
	}

	std::uint32_t model_idx( const char* name ) {
		using getmdlidx_fn = std::uint32_t( __thiscall* )( void* , const char* );
		return vfunc< getmdlidx_fn >( this , 2 )( this , name );
	}

	const char* model_name( void* mdl ) {
		using getmdlname_fn = const char* ( __thiscall* )( void* , void* );
		return vfunc< getmdlname_fn >( this , 3 )( this , mdl );
	}

	studiohdr_t* studio_model( void* mdl ) {
		using getstudiomdl_fn = studiohdr_t * ( __thiscall* )( void* , void* );
		return vfunc< getstudiomdl_fn >( this , 32 )( this , mdl );
	}
};