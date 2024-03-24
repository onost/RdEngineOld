#include "ParticleManager.h"
#include "Graphics/Camera.h"
#include "Graphics/Model/Model.h"
#include "ParticleCommon.h"

void ParticleManager::Create()
{
	mSBuff = std::make_unique<StructuredBuffer>();
	mSBuff->Create(sizeof(Constant), kMaxParticle);
}

void ParticleManager::Update(float deltaTime)
{
	mEmitter.Update(deltaTime);
	// パーティクル放出
	while (mEmitter.mFrequencyTime >= mEmitter.mFrequency)
	{
		mParticles.splice(mParticles.end(), mEmitter.Emit());
		mEmitter.mFrequencyTime -= mEmitter.mFrequency;
	}

	// パーティクル更新
	mCount = 0;
	Constant* data = (Constant*)mSBuff->GetData();
	for (auto it = mParticles.begin(); it != mParticles.end();)
	{
		it->mCurrTime += deltaTime;
		it->mVel += it->mAccel;
		it->mPosition += it->mVel * deltaTime;
		float t = it->mCurrTime / it->mLife;
		it->mScale = MyMath::Lerp<float>(it->mSScale, it->mEScale, t);
		it->mRot = MyMath::Lerp<Vector3>(it->mSRot, it->mERot, t);
		it->mColor = MyMath::Lerp<Color>(it->mSColor, it->mEColor, t);
		if (mCount < kMaxParticle)
		{
			data[mCount].mWorld =
				Matrix4::CreateScale(it->mScale) *
				Matrix4::CreateRotateX(it->mRot.x) *
				Matrix4::CreateRotateY(it->mRot.y) *
				Matrix4::CreateRotateZ(it->mRot.z);
			if (mIsBillboard && ParticleCommon::mCamera)
			{
				data[mCount].mWorld *= ParticleCommon::mCamera->GetBillboard();// ビルボード
			}
			data[mCount].mWorld *= Matrix4::CreateTranslate(it->mPosition);
			data[mCount].mColor = it->mColor;
			++mCount;
		}

		// パーティクル削除
		if (it->mCurrTime >= it->mLife)
		{
			it = mParticles.erase(it);
			--mCount;
		}
		else
		{
			++it;
		}
	}
}

void ParticleManager::Draw(Model* model)
{
	MY_ASSERT(ParticleCommon::mCmdList);
	for (auto& mesh : model->GetMeshes())
	{
		mSBuff->Bind(ParticleCommon::mCmdList, 0);
		mesh->DrawInstancing(ParticleCommon::mCmdList, mCount, 2, 3);
	}
}
