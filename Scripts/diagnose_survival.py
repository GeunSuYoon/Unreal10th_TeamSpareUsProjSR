import unreal


MAP = "/Game/Blueprint/GeunSuYoon/TestGame/Lv04_TestFinal"


def value(obj, name):
    try:
        return obj.get_editor_property(name)
    except Exception as exc:
        return f"<error: {exc}>"


world = unreal.EditorLoadingAndSavingUtils.load_map(MAP)
actors = unreal.EditorLevelLibrary.get_all_level_actors()
unreal.log_warning(f"[SurvivalDiagnostic] map={MAP} actor_count={len(actors)}")

for actor in actors:
    class_name = actor.get_class().get_name()
    if "SurvivalLoop" not in class_name and "SpaceShip" not in class_name and "PlayerStart" not in class_name:
        continue

    unreal.log_warning(
        f"[SurvivalDiagnostic] actor={actor.get_actor_label()} class={class_name} "
        f"location={actor.get_actor_location()}"
    )
    if "SurvivalLoop" in class_name:
        maps = value(actor, "maps")
        unreal.log_warning(
            f"[SurvivalDiagnostic] auto_start={value(actor, 'auto_start')} "
            f"day_duration={value(actor, 'day_duration')} final_day={value(actor, 'final_day')} "
            f"space_ship={value(actor, 'space_ship')} player={value(actor, 'player')} "
            f"prep_timeout={value(actor, 'day_preparation_timeout')} "
            f"minimum_ratio={value(actor, 'minimum_initial_spawn_ratio')} maps={maps}"
        )
        for entry in maps:
            map_data = entry.get_editor_property("map_data")
            rates = value(map_data, "item_spawn_rate") if map_data else None
            unreal.log_warning(
                f"[SurvivalDiagnostic] map_data={map_data} "
                f"init={value(map_data, 'item_spawn_init_count') if map_data else None} "
                f"item_time={value(map_data, 'item_spawn_time') if map_data else None} "
                f"distance={value(map_data, 'item_spawn_dist') if map_data else None} "
                f"speed={value(map_data, 'item_move_speed') if map_data else None} "
                f"meteor_data={value(map_data, 'meteor_data') if map_data else None} "
                f"rates={rates}"
            )

world_settings = world.get_world_settings()
unreal.log_warning(
    f"[SurvivalDiagnostic] default_game_mode={value(world_settings, 'default_game_mode')}"
)

game_mode_class = value(world_settings, "default_game_mode")
if game_mode_class and not isinstance(game_mode_class, str):
    game_mode_cdo = unreal.get_default_object(game_mode_class)
    default_pawn_class = value(game_mode_cdo, "default_pawn_class")
    unreal.log_warning(
        f"[SurvivalDiagnostic] game_mode_cdo={game_mode_cdo} "
        f"default_pawn_class={default_pawn_class} "
        f"player_controller_class={value(game_mode_cdo, 'player_controller_class')} "
        f"hud_class={value(game_mode_cdo, 'hud_class')}"
    )
    if default_pawn_class and not isinstance(default_pawn_class, str):
        pawn_cdo = unreal.get_default_object(default_pawn_class)
        unreal.log_warning(
            f"[SurvivalDiagnostic] pawn_cdo={pawn_cdo} "
            f"pawn_parent={default_pawn_class.get_super_class()}"
        )
