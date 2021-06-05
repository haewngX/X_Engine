#include "AIPolicy.h"
#include "BehaviorTree.h"
#include "Math.h"
#include "ShootActor.h"
#include "MoveComponent.h"
#include "SkeletalMeshComponent.h"
#include "Game.h"
#include "GunActor.h"
#include <iostream>

AIPolicy::AIPolicy(Actor* owner)
    :mActor(owner)
{
    CreateBehavior();
}

AIPolicy::~AIPolicy()
{
    for (auto node : mNodes)
    {
        delete node;
    }
}

void AIPolicy::Action(json& obs)
{
    mObs = obs;
    mRoot->Run(this);
}

void AIPolicy::CreateBehavior()
{
    mRoot = new Selector();
    Node* mSeePlayerNode = new CanSeePlayer();
    Node* mSeePlayerSelector = new Selector();
    Node* mStayInPlace = new StayInPlace();
    Node* mInWeaponRangeNode = new InWeaponRange();
    Node* mShootNode = new Shoot();
    Node* mChaseNode = new Chase();
    
    mRoot->AddChild(mSeePlayerNode);
    mRoot->AddChild(mStayInPlace);
    mSeePlayerNode->AddChild(mSeePlayerSelector);
    mSeePlayerSelector->AddChild(mInWeaponRangeNode);
    mSeePlayerSelector->AddChild(mChaseNode);
    mInWeaponRangeNode->AddChild(mShootNode);

    RecordNode(mRoot);
}

void AIPolicy::RecordNode(class Node* node)
{
    mNodes.push_back(node);
    for (auto child : node->mChildren)
    {
        RecordNode(child);
    }
}

Chase::Chase()
{
}

Status Chase::mRunFunc(AIPolicy* policy)
{
    //std::cout << mName << std::endl;

    Vector3 TarPos(policy->mObs["AI"]["TargetPos_x"], policy->mObs["AI"]["TargetPos_y"], policy->mObs["AI"]["TargetPos_z"]);
    ShootActor* mShootActor = dynamic_cast<ShootActor*>(policy->GetActor());
    if (mShootActor)
    {
        mShootActor->MoveTo(TarPos);
    }
    return Success;
}

Shoot::Shoot()
{
}

Status Shoot::mRunFunc(AIPolicy* policy)
{
    //std::cout << mName << std::endl;
 
    Vector3 TarPos(policy->mObs["AI"]["TargetPos_x"], policy->mObs["AI"]["TargetPos_y"], policy->mObs["AI"]["TargetPos_z"]);
    ShootActor* mShootActor = dynamic_cast<ShootActor*>(policy->GetActor());
    if (mShootActor)
    {
        mShootActor->RotateTo(TarPos);
        if (mShootActor->ShootTime == 0)
        {
            //std::cout << "Shoot" << std::endl;
            mShootActor->Shoot(TarPos);
            mShootActor->ShootTime++;
        }
        else if (mShootActor->ShootTime < mShootActor->ShootInterval)
        {
            mShootActor->ShootTime++;
        }
        else if (mShootActor->ShootTime == mShootActor->ShootInterval)
        {
            mShootActor->ShootTime=0;
        }
    }

    return Success;
}

StayInPlace::StayInPlace()
{
}

Status StayInPlace::mRunFunc(AIPolicy* policy)
{
    //std::cout << mName << std::endl;
    ShootActor* mShootActor = dynamic_cast<ShootActor*>(policy->GetActor());
    if (mShootActor)
    {
        mShootActor->GetMoveComp()->SetAngularSpeed(0.0f);
        mShootActor->GetMoveComp()->SetForwardSpeed(0.0f);
        mShootActor->GetMoveComp()->SetStrafeSpeed(0.0f);
        mShootActor->GetMoveComp()->SetUpSpeed(0.0f);
        if (mShootActor->DeadAnmiTime >= 1 && mShootActor->DeadAnmiTime<30)
        {
            
        }
        else
        {
            mShootActor->mActionState = Idle;
            mShootActor->GetMeshComp()->PlayAnimation(mShootActor->GetGame()->GetAnimation("Assets/Animations/Idle_Rifle_Ironsights.gpanim"));
        }
        
        
    }
    return Success;

}

CanSeePlayer::CanSeePlayer()
{
    
}

Status CanSeePlayer::mRunFunc(AIPolicy* policy)
{
    //std::cout << mName << std::endl;
    if (policy->mObs["AI"]["TargetPos_x"] != Math::Infinity)
    {
        return Success;
    }

    return Fail;

}

InWeaponRange::InWeaponRange()
{
}

Status InWeaponRange::mRunFunc(AIPolicy* policy)
{
    
    //std::cout << mName << std::endl;
    ShootActor* mShootActor = dynamic_cast<ShootActor*>(policy->GetActor());
    if (mShootActor)
    {
        Vector3 TarPos(policy->mObs["AI"]["TargetPos_x"], policy->mObs["AI"]["TargetPos_y"], policy->mObs["AI"]["TargetPos_z"]);
        Vector3 mPos = mShootActor->GetPosition();
        float distance = Math::Sqrt((TarPos.x - mPos.x) * (TarPos.x - mPos.x) + (TarPos.y - mPos.y) * (TarPos.y - mPos.y) + (TarPos.z - mPos.z) * (TarPos.z - mPos.z));
        if (distance <= mShootActor->GunRange)
        {
            return Success;
        }
    }
    return Fail;
}
