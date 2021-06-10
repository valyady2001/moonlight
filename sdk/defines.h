#pragma once
#include <algorithm>
#include <windows.h>
template < typename t >
__forceinline t vfunc( void* thisptr , std::uintptr_t idx ) {
	return reinterpret_cast< t >( ( *reinterpret_cast< std::uintptr_t** >( thisptr ) ) [ idx ] );
}
#define OFFSET( type, offset )		( *( type* ) ( ( std::uintptr_t ) this + ( offset ) ) )


template <typename T , typename ... args_t>
inline constexpr T CallVFunc( void* thisptr , std::size_t nIndex , args_t... argList )
{
	using VirtualFn = T( __thiscall* )( void* , decltype( argList )... );
	return ( *static_cast< VirtualFn** >( thisptr ) ) [ nIndex ]( thisptr , argList... );
}
#define in_attack        (1 << 0)
#define in_jump          (1 << 1)
#define in_duck          (1 << 2)
#define in_forward       (1 << 3)
#define in_back          (1 << 4)
#define in_use           (1 << 5)
#define in_cancel        (1 << 6)
#define in_left          (1 << 7)
#define in_right         (1 << 8)
#define in_moveleft      (1 << 9)
#define in_moveright     (1 << 10)
#define in_attack2       (1 << 11)
#define in_run           (1 << 12)
#define in_reload        (1 << 13)
#define in_alt1          (1 << 14)
#define in_alt2          (1 << 15)
#define in_score         (1 << 16) 
#define in_speed         (1 << 17) 
#define in_walk          (1 << 18) 
#define in_zoom          (1 << 19) 
#define in_weapon1       (1 << 20) 
#define in_weapon2       (1 << 21) 
#define in_bullrush      (1 << 22)
#define in_grenade1      (1 << 23) 
#define in_grenade2      (1 << 24) 


enum item_def_idx
{
	none = 0 ,
	deagle ,
	elite ,
	fiveseven ,
	glock ,
	ak47 = 7 ,
	aug ,
	awp ,
	famas ,
	g3sg1 ,
	galil = 13 ,
	m249 ,
	m4a4 = 16 ,
	mac10 ,
	p90 = 19 ,
	zone_repulsor = 20 ,
	mp5sd = 23 ,
	ump45 ,
	xm1014 ,
	bizon ,
	mag7 ,
	negev ,
	sawedoff ,
	tec9 ,
	zeus ,
	p2000 ,
	mp7 ,
	mp9 ,
	nova ,
	p250 ,
	shield ,
	scar20 ,
	sg553 ,
	ssg08 ,
	knifegg ,
	knife ,
	flashbang ,
	hegrenade ,
	smokegrenade ,
	molotov ,
	decoy ,
	inc ,
	c4 ,
	healthshot = 57 ,
	knife_t = 59 ,
	m4a1s ,
	usps ,
	cz75 = 63 ,
	revolver ,
	tagrenade = 68 ,
	fists ,
	breachcharge ,
	tablet = 72 ,
	melee = 74 ,
	axe ,
	hammer ,
	spanner = 78 ,
	knife_ghost = 80 ,
	firebomb ,
	diversion ,
	frag_grenade ,
	snowball ,
	bumpmine ,
	knife_bayonet = 500 ,
	knife_css = 503 ,
	knife_flip = 505 ,
	knife_gut ,
	knife_karambit ,
	knife_m9_bayonet ,
	knife_tactical ,
	knife_falchion = 512 ,
	knife_survival_bowie = 514 ,
	knife_butterfly ,
	knife_push ,
	knife_cord = 517 ,
	knife_canis = 518 ,
	knife_ursus = 519 ,
	knife_gypsy_jackknife ,
	knife_outdoor ,
	knife_stiletto = 522 ,
	knife_widowmaker ,
	knife_skeleton = 525 ,
	glove_studded_bloodhound = 5027 ,
	glove_t_side = 5028 ,
	glove_ct_side = 5029 ,
	glove_sporty = 5030 ,
	glove_slick = 5031 ,
	glove_leather_wrap = 5032 ,
	glove_motorcycle = 5033 ,
	glove_specialist = 5034 ,
	glove_hydra = 5035
};

enum ent_flags : int
{
	fl_onground = ( 1 << 0 ) ,
	fl_ducking = ( 1 << 1 ) ,
	fl_waterjump = ( 1 << 2 ) ,
	fl_ontrain = ( 1 << 3 ) ,
	fl_inrain = ( 1 << 4 ) ,
	fl_frozen = ( 1 << 5 ) ,
	fl_atcontrols = ( 1 << 6 ) ,
	fl_client = ( 1 << 7 ) ,
	fl_fakeclient = ( 1 << 8 ) ,
	fl_inwater = ( 1 << 9 ) ,
	fl_fly = ( 1 << 10 ) ,
	fl_swim = ( 1 << 11 ) ,
	fl_conveyor = ( 1 << 12 ) ,
	fl_npc = ( 1 << 13 ) ,
	fl_godmode = ( 1 << 14 ) ,
	fl_notarget = ( 1 << 15 ) ,
	fl_aimtarget = ( 1 << 16 ) ,
	fl_partialground = ( 1 << 17 ) ,
	fl_staticprop = ( 1 << 18 ) ,
	fl_graphed = ( 1 << 19 ) ,
	fl_grenade = ( 1 << 20 ) ,
	fl_stepmovement = ( 1 << 21 ) ,
	fl_donttouch = ( 1 << 22 ) ,
	fl_basevelocity = ( 1 << 23 ) ,
	fl_worldbrush = ( 1 << 24 ) ,
	fl_object = ( 1 << 25 ) ,
	fl_killme = ( 1 << 26 ) ,
	fl_onfire = ( 1 << 27 ) ,
	fl_dissolving = ( 1 << 28 ) ,
	fl_transragdoll = ( 1 << 29 ) ,
	fl_unblockable_by_player = ( 1 << 30 )
};

enum effects_t : int {
	ef_bonemerge = 0x1 ,
	ef_brightlight = 0x2 ,
	ef_dimlight = 0x4 ,
	ef_nointerp = 0x8 ,
	ef_noshadow = 0x10 ,
	ef_nodraw = 0x20 ,
	ef_noreceiveshadow = 0x40 ,
	ef_bonemerge_fastcull = 0x80 ,
	ef_item_blink = 0x100 ,
	ef_parent_animates = 0x200 ,
	ef_max_bits = 10
};
enum eflags_t : int
{
	efl_killme = ( 1 << 0 ) ,
	efl_dormant = ( 1 << 1 ) ,
	efl_noclip_active = ( 1 << 2 ) ,
	efl_setting_up_bones = ( 1 << 3 ) ,
	efl_keep_on_recreate_entities = ( 1 << 4 ) ,
	efl_has_player_child = ( 1 << 4 ) ,
	efl_dirty_shadowupdate = ( 1 << 5 ) ,
	efl_notify = ( 1 << 6 ) ,
	efl_force_check_transmit = ( 1 << 7 ) ,
	efl_bot_frozen = ( 1 << 8 ) ,
	efl_server_only = ( 1 << 9 ) ,
	efl_no_auto_edict_attach = ( 1 << 10 ) ,
	efl_dirty_abstransform = ( 1 << 11 ) ,
	efl_dirty_absvelocity = ( 1 << 12 ) ,
	efl_dirty_absangvelocity = ( 1 << 13 ) ,
	efl_dirty_surrounding_collision_bounds = ( 1 << 14 ) ,
	efl_dirty_spatial_partition = ( 1 << 15 ) ,
	efl_in_skybox = ( 1 << 17 ) ,
	efl_use_partition_when_not_solid = ( 1 << 18 ) ,
	efl_touching_fluid = ( 1 << 19 ) ,
	efl_is_being_ted_by_barnacle = ( 1 << 20 ) ,
	efl_no_rotorwash_push = ( 1 << 21 ) ,
	efl_no_think_function = ( 1 << 22 ) ,
	efl_no_game_physics_simulation = ( 1 << 23 ) ,
	efl_check_untouch = ( 1 << 24 ) ,
	efl_dontblocklos = ( 1 << 25 ) ,
	efl_dontwalkon = ( 1 << 26 ) ,
	efl_no_dissolve = ( 1 << 27 ) ,
	efl_no_megaphyscannon_ragdoll = ( 1 << 28 ) ,
	efl_no_water_velocity_change = ( 1 << 29 ) ,
	efl_no_physcannon_interaction = ( 1 << 30 ) ,
	efl_no_damage_forces = ( 1 << 31 )
};

enum PoseParam_t {
	strafe_yaw , //-180, 180
	stand , //0, 1
	lean_yaw , //-180,180
	speed , // 0,1
	ladder_yaw , //-180, 180
	ladder_speed , //0,1
	jump_fall , // 0,1
	move_yaw , //-180, 180
	move_blend_crouch , // 0 ,1
	move_blend_walk , // 0,1
	move_blend_run ,// 0,1
	body_yaw , //-60,60
	body_pitch , // -90,90
	aim_blend_stand_idle , //0,1
	aim_blend_stand_walk ,//0,1
	aim_blend_stand_run ,//0,1
	aim_blend_courch_idle ,//0,1
	aim_blend_crouch_walk ,//0,1
	death_yaw //-180,180
};

enum EClassIds : int
{
	CAI_BaseNPC = 0 ,
	CAK47 ,
	CBaseAnimating ,
	CBaseAnimatingOverlay ,
	CBaseAttributableItem ,
	CBaseButton ,
	CBaseCombatCharacter ,
	CBaseCombatWeapon ,
	CBaseCSGrenade ,
	CBaseCSGrenadeProjectile ,
	CBaseDoor ,
	CBaseEntity ,
	CBaseFlex ,
	CBaseGrenade ,
	CBaseParticleEntity ,
	CBasePlayer ,
	CBasePropDoor ,
	CBaseTeamObjectiveResource ,
	CBaseTempEntity ,
	CBaseToggle ,
	CBaseTrigger ,
	CBaseViewModel ,
	CBaseVPhysicsTrigger ,
	CBaseWeaponWorldModel ,
	CBeam ,
	CBeamSpotlight ,
	CBoneFollower ,
	CBRC4Target ,
	CBreachCharge ,
	CBreachChargeProjectile ,
	CBreakableProp ,
	CBreakableSurface ,
	CBumpMine ,
	CBumpMineProjectile ,
	CC4 ,
	CCascadeLight ,
	CChicken ,
	CColorCorrection ,
	CColorCorrectionVolume ,
	CCSGameRulesProxy ,
	CCSPlayer ,
	CCSPlayerResource ,
	CCSRagdoll ,
	CCSTeam ,
	CDangerZone ,
	CDangerZoneController ,
	CDEagle ,
	CDecoyGrenade ,
	CDecoyProjectile ,
	CDrone ,
	CDronegun ,
	CDynamicLight ,
	CDynamicProp ,
	CEconEntity ,
	CEconWearable ,
	CEmbers ,
	CEntityDissolve ,
	CEntityFlame ,
	CEntityFreezing ,
	CEntityParticleTrail ,
	CEnvAmbientLight ,
	CEnvDetailController ,
	CEnvDOFController ,
	CEnvGasCanister ,
	CEnvParticleScript ,
	CEnvProjectedTexture ,
	CEnvQuadraticBeam ,
	CEnvScreenEffect ,
	CEnvScreenOverlay ,
	CEnvTonemapController ,
	CEnvWind ,
	CFEPlayerDecal ,
	CFireCrackerBlast ,
	CFireSmoke ,
	CFireTrail ,
	CFish ,
	CFists ,
	CFlashbang ,
	CFogController ,
	CFootstepControl ,
	CFunc_Dust ,
	CFunc_LOD ,
	CFuncAreaPortalWindow ,
	CFuncBrush ,
	CFuncConveyor ,
	CFuncLadder ,
	CFuncMonitor ,
	CFuncMoveLinear ,
	CFuncOccluder ,
	CFuncReflectiveGlass ,
	CFuncRotating ,
	CFuncSmokeVolume ,
	CFuncTrackTrain ,
	CGameRulesProxy ,
	CGrassBurn ,
	CHandleTest ,
	CHEGrenade ,
	CHostage ,
	CHostageCarriableProp ,
	CIncendiaryGrenade ,
	CInferno ,
	CInfoLadderDismount ,
	CInfoMapRegion ,
	CInfoOverlayAccessor ,
	CItem_Healthshot ,
	CItemCash ,
	CItemDogtags ,
	CKnife ,
	CKnifeGG ,
	CLightGlow ,
	CMaterialModifyControl ,
	CMelee ,
	CMolotovGrenade ,
	CMolotovProjectile ,
	CMovieDisplay ,
	CParadropChopper ,
	CParticleFire ,
	CParticlePerformanceMonitor ,
	CParticleSystem ,
	CPhysBox ,
	CPhysBoxMultiplayer ,
	CPhysicsProp ,
	CPhysicsPropMultiplayer ,
	CPhysMagnet ,
	CPhysPropAmmoBox ,
	CPhysPropLootCrate ,
	CPhysPropRadarJammer ,
	CPhysPropWeaponUpgrade ,
	CPlantedC4 ,
	CPlasma ,
	CPlayerPing ,
	CPlayerResource ,
	CPointCamera ,
	CPointCommentaryNode ,
	CPointWorldText ,
	CPoseController ,
	CPostProcessController ,
	CPrecipitation ,
	CPrecipitationBlocker ,
	CPredictedViewModel ,
	CProp_Hallucination ,
	CPropCounter ,
	CPropDoorRotating ,
	CPropJeep ,
	CPropVehicleDriveable ,
	CRagdollManager ,
	CRagdollProp ,
	CRagdollPropAttached ,
	CRopeKeyframe ,
	CSCAR17 ,
	CSceneEntity ,
	CSensorGrenade ,
	CSensorGrenadeProjectile ,
	CShadowControl ,
	CSlideshowDisplay ,
	CSmokeGrenade ,
	CSmokeGrenadeProjectile ,
	CSmokeStack ,
	CSnowball ,
	CSnowballPile ,
	CSnowballProjectile ,
	CSpatialEntity ,
	CSpotlightEnd ,
	CSprite ,
	CSpriteOriented ,
	CSpriteTrail ,
	CStatueProp ,
	CSteamJet ,
	CSun ,
	CSunlightShadowControl ,
	CSurvivalSpawnChopper ,
	CTablet ,
	CTeam ,
	CTeamplayRoundBasedRulesProxy ,
	CTEArmorRicochet ,
	CTEBaseBeam ,
	CTEBeamEntPoint ,
	CTEBeamEnts ,
	CTEBeamFollow ,
	CTEBeamLaser ,
	CTEBeamPoints ,
	CTEBeamRing ,
	CTEBeamRingPoint ,
	CTEBeamSpline ,
	CTEBloodSprite ,
	CTEBloodStream ,
	CTEBreakModel ,
	CTEBSPDecal ,
	CTEBubbles ,
	CTEBubbleTrail ,
	CTEClientProjectile ,
	CTEDecal ,
	CTEDust ,
	CTEDynamicLight ,
	CTEEffectDispatch ,
	CTEEnergySplash ,
	CTEExplosion ,
	CTEFireBullets ,
	CTEFizz ,
	CTEFootprintDecal ,
	CTEFoundryHelpers ,
	CTEGaussExplosion ,
	CTEGlowSprite ,
	CTEImpact ,
	CTEKillPlayerAttachments ,
	CTELargeFunnel ,
	CTEMetalSparks ,
	CTEMuzzleFlash ,
	CTEParticleSystem ,
	CTEPhysicsProp ,
	CTEPlantBomb ,
	CTEPlayerAnimEvent ,
	CTEPlayerDecal ,
	CTEProjectedDecal ,
	CTERadioIcon ,
	CTEShatterSurface ,
	CTEShowLine ,
	CTesla ,
	CTESmoke ,
	CTESparks ,
	CTESprite ,
	CTESpriteSpray ,
	CTest_ProxyToggle_Networkable ,
	CTestTraceline ,
	CTEWorldDecal ,
	CTriggerPlayerMovement ,
	CTriggerSoundOperator ,
	CVGuiScreen ,
	CVoteController ,
	CWaterBullet ,
	CWaterLODControl ,
	CWeaponAug ,
	CWeaponAWP ,
	CWeaponBaseItem ,
	CWeaponBizon ,
	CWeaponCSBase ,
	CWeaponCSBaseGun ,
	CWeaponCycler ,
	CWeaponElite ,
	CWeaponFamas ,
	CWeaponFiveSeven ,
	CWeaponG3SG1 ,
	CWeaponGalil ,
	CWeaponGalilAR ,
	CWeaponGlock ,
	CWeaponHKP2000 ,
	CWeaponM249 ,
	CWeaponM3 ,
	CWeaponM4A1 ,
	CWeaponMAC10 ,
	CWeaponMag7 ,
	CWeaponMP5Navy ,
	CWeaponMP7 ,
	CWeaponMP9 ,
	CWeaponNegev ,
	CWeaponNOVA ,
	CWeaponP228 ,
	CWeaponP250 ,
	CWeaponP90 ,
	CWeaponSawedoff ,
	CWeaponSCAR20 ,
	CWeaponScout ,
	CWeaponSG550 ,
	CWeaponSG552 ,
	CWeaponSG556 ,
	CWeaponShield ,
	CWeaponSSG08 ,
	CWeaponTaser ,
	CWeaponTec9 ,
	CWeaponTMP ,
	CWeaponUMP45 ,
	CWeaponUSP ,
	CWeaponXM1014 ,
	CWorld ,
	CWorldVguiText ,
	DustTrail ,
	MovieExplosion ,
	ParticleSmokeGrenade ,
	RocketTrail ,
	SmokeTrail ,
	SporeExplosion ,
	SporeTrail ,
};


enum class weapon_type : int
{
	type_knife = 0 ,
	type_pistol ,
	type_submachinegun ,
	type_rifle ,
	type_shotgun ,
	type_sniper_rifle ,
	type_machinegun ,
	type_c4 ,
	type_placeholder ,
	type_grenade ,
	type_unknown
};
enum class move_type
{
	type_none = 0 ,
	type_isometric ,
	type_walk ,
	type_step ,
	type_fly ,
	type_flygravity ,
	type_vphysics ,
	type_push ,
	type_noclip ,
	type_ladder ,
	type_observer ,
	type_custom ,
	type_last = type_custom ,
	type_max_bits = 4
};

#define COMBINE( x, y ) x##y
#define COMBINE2( x, y ) COMBINE( x, y )

#define PAD( sz ) \
	private: \
	std::uint8_t COMBINE2( pad_, __COUNTER__ )[ sz ]; \
	public:


inline static void dbg_print( const char* msg , ... ) {
	if ( !msg )
		return;

	static void( __cdecl * msg_fn )( const char* , va_list ) = ( decltype( msg_fn ) ) ( GetProcAddress( GetModuleHandleA( "tier0.dll" ) , "Msg" ) ); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer [ 989 ];
	va_list list;
	va_start( list , msg );
	vsprintf_s( buffer , msg , list );
	va_end( list );
	msg_fn( buffer , list );
}

enum hitboxes : int
{
	head ,
	neck ,
	pelvis ,
	stomach ,
	chest ,
	lower_chest ,
	upper_chest ,
	right_thigh ,
	left_thigh ,
	right_calf ,
	left_calf ,
	right_foot ,
	left_foot ,
	right_hand ,
	left_hand ,
	right_upper_arm ,
	right_forearm ,
	left_upper_arm ,
	left_forearm ,
	max
};

class weapon_info_t
{
public:
	PAD( 0x14 );
	uint32_t max_clip;
	PAD( 0xC );
	uint32_t max_reserved_ammo;
	PAD( 0x60 );
	char* hud_name;
	char* weapon_name;
	PAD( 0x38 );
	uint32_t type;
	PAD( 0x4 );
	uint32_t price;
	uint32_t reward;
	PAD( 0x4 );
	float_t fire_rate;
	PAD( 0xC );
	uint8_t full_auto;
	PAD( 0x3 );
	uint32_t dmg;
	float_t armor_ratio;
	uint32_t bullets;
	float_t penetration;
	PAD( 0x8 );
	float_t range;
	float_t range_modifier;
	PAD( 0x10 );
	uint8_t has_silencer;
	PAD( 0xF );
	float_t max_speed;
	float_t max_speed_alt;
	PAD( 0x4C );
	uint32_t recoil_seed;
	PAD( 0x20 );
};

enum ECollisionGroup : int
{
	COLLISION_GROUP_NONE = 0 ,
	COLLISION_GROUP_DEBRIS ,						// collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER ,				// same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS ,			// collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE ,				// collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER ,
	COLLISION_GROUP_BREAKABLE_GLASS ,
	COLLISION_GROUP_VEHICLE ,
	COLLISION_GROUP_PLAYER_MOVEMENT ,			// for HL2, same as Collision_Group_Player, for / TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC ,						// generic NPC group
	COLLISION_GROUP_IN_VEHICLE ,					// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON ,						// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP ,				// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE ,					// projectiles!
	COLLISION_GROUP_DOOR_BLOCKER ,				// blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR ,				// doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING ,					// things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY ,					// nonsolid on client and server, pushaway in player code
	COLLISION_GROUP_NPC_ACTOR ,					// used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED ,				// used for NPCs in scripts that should not collide with each other
	COLLISION_GROUP_PZ_CLIP ,
	COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE ,	// only collides with bullets
	LAST_SHARED_COLLISION_GROUP
};
#pragma endregion


enum EMultiplayerPhysicsMode : int
{
	PHYSICS_MULTIPLAYER_AUTODETECT = 0 ,	// use multiplayer physics mode as defined in model prop data
	PHYSICS_MULTIPLAYER_SOLID = 1 ,	// solid, pushes player away
	PHYSICS_MULTIPLAYER_NON_SOLID = 2 ,	// nonsolid, but pushed by player
	PHYSICS_MULTIPLAYER_CLIENTSIDE = 3	// clientside only, nonsolid
};

class IMultiplayerPhysics
{
public:
	virtual int		GetMultiplayerPhysicsMode( ) = 0;
	virtual float	GetMass( ) = 0;
	virtual bool	IsAsleep( ) = 0;
};


enum EPropertyDataInteractions : int
{
	PROPINTER_PHYSGUN_WORLD_STICK ,		// "onworldimpact"		"stick"
	PROPINTER_PHYSGUN_FIRST_BREAK ,		// "onfirstimpact"		"break"
	PROPINTER_PHYSGUN_FIRST_PAINT ,		// "onfirstimpact"		"paintsplat"
	PROPINTER_PHYSGUN_FIRST_IMPALE ,		// "onfirstimpact"		"impale"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_NONE ,	// "onlaunch"			"spin_none"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_Z ,	// "onlaunch"			"spin_zaxis"
	PROPINTER_PHYSGUN_BREAK_EXPLODE ,	// "onbreak"			"explode_fire"
	PROPINTER_PHYSGUN_BREAK_EXPLODE_ICE ,// "onbreak"			"explode_ice"
	PROPINTER_PHYSGUN_DAMAGE_NONE ,		// "damage"				"none"
	PROPINTER_FIRE_FLAMMABLE ,			// "flammable"			"yes"
	PROPINTER_FIRE_EXPLOSIVE_RESIST ,	// "explosive_resist"	"yes"
	PROPINTER_FIRE_IGNITE_HALFHEALTH ,	// "ignite"				"halfhealth"
	PROPINTER_PHYSGUN_CREATE_FLARE ,		// "onpickup"			"create_flare"
	PROPINTER_PHYSGUN_ALLOW_OVERHEAD ,	// "allow_overhead"		"yes"
	PROPINTER_WORLD_BLOODSPLAT ,			// "onworldimpact",		"bloodsplat"
	PROPINTER_PHYSGUN_NOTIFY_CHILDREN ,	// "onfirstimpact" - cause attached flechettes to explode
	PROPINTER_MELEE_IMMUNE ,				// "melee_immune"		"yes"
	PROPINTER_NUM_INTERACTIONS			// if we get more than 32 of these, we'll need a different system
};

enum EMultiplayerBreak : int
{
	MULTIPLAYER_BREAK_DEFAULT ,
	MULTIPLAYER_BREAK_SERVERSIDE ,
	MULTIPLAYER_BREAK_CLIENTSIDE ,
	MULTIPLAYER_BREAK_BOTH
};
class IBreakableWithPropData
{
public:
	// Damage modifiers
	virtual void		SetDmgModBullet( float flDmgMod ) = 0;
	virtual void		SetDmgModClub( float flDmgMod ) = 0;
	virtual void		SetDmgModExplosive( float flDmgMod ) = 0;
	virtual float		GetDmgModBullet( ) = 0;
	virtual float		GetDmgModClub( ) = 0;
	virtual float		GetDmgModExplosive( ) = 0;
	virtual float		GetDmgModFire( ) = 0;

	// Explosive
	virtual void		SetExplosiveRadius( float flRadius ) = 0;
	virtual void		SetExplosiveDamage( float flDamage ) = 0;
	virtual float		GetExplosiveRadius( ) = 0;
	virtual float		GetExplosiveDamage( ) = 0;

	// Physics damage tables
	virtual void		SetPhysicsDamageTable( const char* iszTableName ) = 0;
	virtual const char* GetPhysicsDamageTable( ) = 0;

	// Breakable chunks
	virtual void		SetBreakableModel( const char* iszModel ) = 0;
	virtual const char* 	GetBreakableModel( ) = 0;
	virtual void		SetBreakableSkin( int iSkin ) = 0;
	virtual int			GetBreakableSkin( ) = 0;
	virtual void		SetBreakableCount( int iCount ) = 0;
	virtual int			GetBreakableCount( ) = 0;
	virtual void		SetMaxBreakableSize( int iSize ) = 0;
	virtual int			GetMaxBreakableSize( ) = 0;

	// LOS blocking
	virtual void		SetPropDataBlocksLOS( bool bBlocksLOS ) = 0;
	virtual void		SetPropDataIsAIWalkable( bool bBlocksLOS ) = 0;

	// Interactions
	virtual void		SetInteraction( EPropertyDataInteractions Interaction ) = 0;
	virtual bool		HasInteraction( EPropertyDataInteractions Interaction ) = 0;

	// Multiplayer physics mode
	virtual void		SetPhysicsMode( int iMode ) = 0;
	virtual int			GetPhysicsMode( ) = 0;

	// Multiplayer breakable spawn behavior
	virtual void		SetMultiplayerBreakMode( EMultiplayerBreak mode ) = 0;
	virtual EMultiplayerBreak GetMultiplayerBreakMode( ) const = 0;

	// Used for debugging
	virtual void		SetBasePropData( const char* iszBase ) = 0;
	virtual const char*	GetBasePropData( ) = 0;
};
