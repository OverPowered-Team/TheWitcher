#include "CameraMovement.h"
#include "PlayerController.h"
#include "CutsceneCamera.h"

CameraMovement::CameraMovement() : Alien()
{
}

CameraMovement::~CameraMovement()
{
}

void CameraMovement::Start()
{
    //Get the players in the scene
    SearchAndAssignPlayers();
    trg_offset = CalculateCameraPos(hor_angle, vert_angle, distance);
    transform->SetGlobalPosition(CalculateMidPoint() + trg_offset);
    first_pos = CalculateMidPoint() + trg_offset;
    LOG("TRG_OFFSET X: %f Y: %f Z: %f", trg_offset.x, trg_offset.y, trg_offset.z);
    LOG("FIRSTPOS X: %f Y: %f Z: %f", first_pos.x, first_pos.y, first_pos.z);
    LookAtMidPoint();
    prev_state = CameraState::FREE;
}

void CameraMovement::Update()
{
    switch (state) {
    case CameraState::DYNAMIC: {
        if (is_cinematic)
        {
            prev_state = state;
            state = CameraState::MOVING_TO_CINEMATIC;
        }
        else {
            if (smooth_camera) {
                float3 pos = transform->GetGlobalPosition();
                transform->SetGlobalPosition(pos + ((CalculateMidPoint() + trg_offset) - pos) * smooth_cam_vel * Time::GetDT());
            }
            else
                transform->SetGlobalPosition(CalculateMidPoint() + trg_offset);
        }
        break;
    }
    case CameraState::FREE: {
        if (is_cinematic)
        {
            prev_state = state;
            state = CameraState::MOVING_TO_CINEMATIC;
        }
        else {
            float3 pos = players[closest_player]->transform->GetGlobalPosition() + prev_middle;

            auto frustum = GetComponent<ComponentCamera>()->frustum;
            frustum.pos = pos;
            bool inside = true;
            AABB aabbs[2] = { players[0]->GetComponent<PlayerController>()->max_aabb, players[1]->GetComponent<PlayerController>()->max_aabb };
            for (int i = 0; i < players.size(); ++i)
            {
                aabbs[i].minPoint += players[i]->transform->GetGlobalPosition();
                aabbs[i].maxPoint += players[i]->transform->GetGlobalPosition();
                if (!frustum.Contains(aabbs[i]))
                {
                    inside = false;
                }
            }
            if (inside)
                transform->SetGlobalPosition(transform->GetGlobalPosition() + (pos - transform->GetGlobalPosition()) * smooth_cam_vel * Time::GetDT());

            inside = true;
            pos = CalculateMidPoint() + trg_offset;
            frustum.pos = pos;
            for (int i = 0; i < players.size(); ++i)
            {
                if (!frustum.Contains(aabbs[i]))
                {
                    inside = false;
                }
            }
            if (inside) {
                if (prev_state == CameraState::FREE)
                    prev_state = CameraState::FREE_TO_DYNAMIC;

                state = prev_state;
            }
        }
        break;
    }
    case CameraState::FREE_TO_DYNAMIC: {
        float3 pos = transform->GetGlobalPosition();
        transform->SetGlobalPosition(pos + ((CalculateMidPoint() + trg_offset) - pos) * 1 * Time::GetDT());
        break;
    }
    case CameraState::STATIC:
        if (is_cinematic)
        {
            prev_state = state;
            state = CameraState::MOVING_TO_CINEMATIC;
        }
        else
            LookAtMidPoint();
        break;
    case CameraState::AXIS: {
        if (is_cinematic)
        {
            prev_state = state;
            state = CameraState::MOVING_TO_CINEMATIC;
        }
        else {
            if (smooth_camera)
                transform->SetGlobalPosition(transform->GetGlobalPosition() + ((CalculateAxisMidPoint()) - transform->GetGlobalPosition()) * smooth_cam_vel * Time::GetDT());
            else {
                transform->SetGlobalPosition(CalculateAxisMidPoint()/* + trg_offset*/);
            }
            LookAtMidPoint();
        }
        break;
    }
    case CameraState::MOVING_TO_AXIS: {
        float min_dist = 0.5f;
        if (trg_offset.Distance(transform->GetGlobalPosition()) < min_dist) {
            LOG("Finished transition");
            first_pos = trg_offset;
            transform->SetGlobalPosition(trg_offset);
            trg_offset = transform->GetGlobalPosition() - CalculateMidPoint();
            state = CameraState::AXIS;
        }
        LookAtMidPoint();
        break;
    }
    case CameraState::MOVING_TO_STATIC: {
        float min_dist = 0.1f;
        if (trg_offset.Distance(transform->GetGlobalPosition()) < min_dist) {
            LOG("Finished transition");
            transform->SetGlobalPosition(trg_offset);
            state = CameraState::STATIC;
        }
        LookAtMidPoint();
        break;
    }
    case CameraState::MOVING_TO_DYNAMIC:
    {
        current_transition_time += Time::GetDT();
        float3 trg_pos = CalculateMidPoint() + trg_offset;
        float3 curr_pos = transform->GetGlobalPosition();
        float min_dist = 0.1f;
        if ((trg_pos - curr_pos).Length() < min_dist) {
            LOG("Finished transition");
            transform->SetGlobalPosition(trg_pos);
            state = CameraState::DYNAMIC;
        }
        else {
            //INFO: This is more like an accelerated movement than a lerp since we're using the current position as the starting point

            float time_percent = (current_transition_time / curr_transition.transition_time);//A value from 0 to 1, 0 meaning it has just started and 1 meaning it has finished

            transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent));//Faster on the beggining

            //TODO: We could add the option to configure smoothness (cuadratic t, cubic t, etc.)
            //transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent * time_percent));//Less fast on the beggining
            //transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent * time_percent * time_percent));//Even less fast on the beggining
        }
        LookAtMidPoint();
        break;
    }
    case CameraState::MOVING_TO_CINEMATIC:
    {
        current_transition_time += Time::GetDT();
        float3 trg_pos = CalculateMidPoint() + cutscene_game_object->transform->GetGlobalPosition();
        float3 curr_pos = transform->GetGlobalPosition();
        float min_dist = 0.1f;
        if ((trg_pos - curr_pos).Length() < min_dist) {
            LOG("Finished transition");
            transform->SetGlobalPosition(trg_pos);
            if (cutscene_game_object) {
                cutscene_game_object->GetComponent<CutsceneCamera>()->PrepareCutscene();
                state = CameraState::CINEMATIC;
            }
        }
        else {
            float time_percent = (current_transition_time / curr_transition.transition_time);//A value from 0 to 1, 0 meaning it has just started and 1 meaning it has finished
            transform->SetGlobalPosition(transform->GetGlobalPosition() + (trg_pos - curr_pos) * (time_percent));//Faster on the beggining
        }
        break;
    }
    case CameraState::CINEMATIC: 
    {
        if(cutscene_game_object)
            cutscene_game_object->GetComponent<CutsceneCamera>()->ExecuteCutscene();
        break;
    }
    }
}

void CameraMovement::LookAtMidPoint()
{
    float3 direction = (CalculateMidPoint() - transform->GetGlobalPosition()).Normalized();

    transform->SetGlobalRotation(Quat::LookAt(float3::unitZ(), direction, float3::unitY(), float3::unitY()));
}

float3 CameraMovement::CalculateCameraPos(const float& ang1, const float& ang2, const float& dst)
{
    float angle1 = math::DegToRad(ang1);
    float angle2 = math::DegToRad(ang2);

    return float3(cos(angle1) * cos(angle2), sin(angle2), sin(angle1) * cos(angle2)) * dst;
}

void CameraMovement::OnDrawGizmos()
{
    float3 mid_point = float3::zero();
    if (players.size() == 0 || search_players) {
        players.clear();
        SearchAndAssignPlayers();
        search_players = false;
    }
    else {
         mid_point = CalculateMidPoint();
    }
    Gizmos::DrawWireSphere(mid_point, .15f, Color::Cyan(), 0.5F); // mid point

    float3 cam_pos = mid_point + CalculateCameraPos(hor_angle, vert_angle, distance);
    Gizmos::DrawWireSphere(cam_pos, 0.15f, Color::Green());

    Gizmos::DrawLine(mid_point, cam_pos, Color::Red()); // line mid -> future camera pos
}

void CameraMovement::SearchAndAssignPlayers()
{
    auto objs = GameObject::FindGameObjectsWithTag("Player");
    for (auto i = objs.begin(); i != objs.end(); i++) {
        if (std::find(players.begin(), players.end(), *i) == players.end())
            players.push_back(*i);
    }
    num_curr_players = objs.size();
    LOG("player num %i", num_curr_players);
}

float3 CameraMovement::CalculateMidPoint()
{
    float3 mid_pos(0,0,0);
    for (std::vector<GameObject*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        if ((*it) != nullptr)
        {
            mid_pos += (*it)->transform->GetGlobalPosition();
        }
       
    }

    return (players.size() == 0) ? mid_pos : mid_pos / players.size();
}

float3 CameraMovement::CalculateAxisMidPoint()
{
    float3 mid_pos(0, 0, 0);
    for (std::vector<GameObject*>::iterator it = players.begin(); it != players.end(); ++it)
    {
        mid_pos += (*it)->transform->GetGlobalPosition();
    }
    switch ((CameraAxis)axis)
    {
    case CameraAxis::X://X
    {
        float3 mid;
        mid = float3((mid_pos.x) * 0.5f, 0, 0);
        return float3(mid.x + trg_offset.x, first_pos.y, first_pos.z);
        break;
    }
    case CameraAxis::Y://Y
    {
        float3 mid;
        mid = float3(0, (mid_pos.y) * 0.5f, 0);
        return float3(first_pos.x, mid.y + trg_offset.y, first_pos.z);
        break;
    }
    case CameraAxis::Z://Z
    {
        float3 mid;
        mid = float3(0, 0, (mid_pos.z) * 0.5f);
        return float3(first_pos.x, first_pos.y, mid.z + trg_offset.z);
        break;
    }
    }
    return mid_pos;
}
Quat CameraMovement::RotationBetweenVectors(math::float3& start, math::float3& dest) // Include in MathGeoLib
{
    start.Normalize();
    dest.Normalize();

    float cosTheta = Dot(start, dest);
    float3 rotationAxis;

    if (cosTheta < -1 + 0.001f) {
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = Cross(float3(0.0f, 0.0f, 1.0f), start);
        if (Length(rotationAxis) < 0.01f) // bad luck, they were parallel, try again!
            rotationAxis = Cross(float3(1.0f, 0.0f, 0.0f), start);

        rotationAxis.Normalize();
        return Quat(rotationAxis, math::DegToRad(180.f));
    }

    rotationAxis = Cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return Quat(
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs,
        s * 0.5f
    );
}
