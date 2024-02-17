#pragma once
#include "Core/StructuredBuffer.h"
#include "Emitter.h"
#include "Matrix4.h"
#include "Particle.h"
#include <list>
#include <memory>

class Model;

class ParticleManager
{
public:
	struct Constant
	{
		Matrix4 mWorld;
		Color mColor;
	};

	void Create();
	void Update(float deltaTime);
	void Draw(Model* model);

	//void IsDead();
	void Reset() { mEmitter.Reset(); }

	// アクセッサ
	const Emitter& GetEmitter() const { return mEmitter; }
	uint32_t GetCount() const { return mCount; }
	bool GetIsBillboard() const { return mIsBillboard; }
	void SetEmitter(const Emitter& emitter) { mEmitter = emitter; }
	void SetIsBillboard(bool isBillboard) { mIsBillboard = isBillboard; }
	// Emitter
	// Getter
	bool GetIsPlaying() const { return mEmitter.mIsPlaying; }
	const Vector3& GetPosition() const { return mEmitter.mPosition; }
	float GetCurrTime() const { return mEmitter.mCurrTime; }
	uint32_t GetAmount() const { return mEmitter.mAmount; }
	float GetFrequency() const { return mEmitter.mFrequency; }
	float GetFrequencyTime() const { return mEmitter.mFrequencyTime; }
	bool GetIsLoop() const { return mEmitter.mIsLoop; }
	float GetDelay() const { return mEmitter.mDelay; }
	Emitter::Shape GetShape() const { return mEmitter.mShape; }
	const Vector3& GetBoxMin() const { return mEmitter.mBoxMin; }
	const Vector3& GetBoxMax() const { return mEmitter.mBoxMax; }
	float GetSphereRadius() const { return mEmitter.mSphereRadius; }
	float GetInitLifeMin() const { return mEmitter.mInitLife.mMin; }
	float GetInitLifeMax() const { return mEmitter.mInitLife.mMax; }
	const Vector3& GetInitVelMin() const { return mEmitter.mInitVel.mMin; }
	const Vector3& GetInitVelMax() const { return mEmitter.mInitVel.mMax; }
	const Vector3& GetInitAccelMin() const { return mEmitter.mInitAccel.mMin; }
	const Vector3& GetInitAccelMax() const { return mEmitter.mInitAccel.mMax; }
	float GetInitSScaleMin() const { return mEmitter.mInitSScale.mMin; }
	float GetInitSScaleMax() const { return mEmitter.mInitSScale.mMax; }
	float GetInitEScaleMin() const { return mEmitter.mInitEScale.mMin; }
	float GetInitEScaleMax() const { return mEmitter.mInitEScale.mMax; }
	const Vector3& GetInitSRotMin() const { return mEmitter.mInitSRot.mMin; }
	const Vector3& GetInitSRotMax() const { return mEmitter.mInitSRot.mMax; }
	const Vector3& GetInitERotMin() const { return mEmitter.mInitERot.mMin; }
	const Vector3& GetInitERotMax() const { return mEmitter.mInitERot.mMax; }
	const Color& GetInitSColorMin() const { return mEmitter.mInitSColor.mMin; }
	const Color& GetInitSColorMax() const { return mEmitter.mInitSColor.mMax; }
	const Color& GetInitEColorMin() const { return mEmitter.mInitEColor.mMin; }
	const Color& GetInitEColorMax() const { return mEmitter.mInitEColor.mMax; }
	// Setter
	void SetIsPlaying(bool isPlaying) { mEmitter.mIsPlaying = isPlaying; }
	void SetPosition(const Vector3& position) { mEmitter.mPosition = position; }
	void SetAmount(uint32_t amount) { mEmitter.mAmount = amount; }
	void SetFrequency(float frequency) { mEmitter.mFrequency = frequency; }
	void SetIsLoop(bool isLoop) { mEmitter.mIsLoop = isLoop; }
	void SetDelay(float delay) { mEmitter.mDelay = delay; }
	void SetShape(Emitter::Shape shape) { mEmitter.mShape = shape; }
	void SetBoxMin(const Vector3& boxMin) { mEmitter.mBoxMin = boxMin; }
	void SetBoxMax(const Vector3& boxMax) { mEmitter.mBoxMin = boxMax; }
	void SetSphereRadius(float sphereRadius) { mEmitter.mSphereRadius = sphereRadius; }
	void SetInitLifeMin(float life) { mEmitter.mInitLife.mMin = life; }
	void SetInitLifeMax(float life) { mEmitter.mInitLife.mMax = life; }
	void SetInitVelMin(const Vector3& vel) { mEmitter.mInitVel.mMin = vel; }
	void SetInitVelMax(const Vector3& vel) { mEmitter.mInitVel.mMax = vel; }
	void SetInitAccelMin(const Vector3& accel) { mEmitter.mInitAccel.mMin = accel; }
	void SetInitAccelMax(const Vector3& accel) { mEmitter.mInitAccel.mMax = accel; }
	void SetInitSScaleMin(float scale) { mEmitter.mInitSScale.mMin = scale; }
	void SetInitSScaleMax(float scale) { mEmitter.mInitSScale.mMax = scale; }
	void SetInitEScaleMin(float scale) { mEmitter.mInitEScale.mMin = scale; }
	void SetInitEScaleMax(float scale) { mEmitter.mInitEScale.mMax = scale; }
	void SetInitSRotMin(const Vector3& rot) { mEmitter.mInitSRot.mMin = rot; }
	void SetInitSRotMax(const Vector3& rot) { mEmitter.mInitSRot.mMax = rot; }
	void SetInitERotMin(const Vector3& rot) { mEmitter.mInitERot.mMin = rot; }
	void SetInitERotMax(const Vector3& rot) { mEmitter.mInitERot.mMax = rot; }
	void SetInitSColorMin(const Color& color) { mEmitter.mInitSColor.mMin = color; }
	void SetInitSColorMax(const Color& color) { mEmitter.mInitSColor.mMax = color; }
	void SetInitEColorMin(const Color& color) { mEmitter.mInitEColor.mMin = color; }
	void SetInitEColorMax(const Color& color) { mEmitter.mInitEColor.mMax = color; }

private:
	static const uint32_t kMaxParticle = 1024;
	std::unique_ptr<StructuredBuffer> mSBuff;
	// エミッター
	Emitter mEmitter;
	// パーティクル
	std::list<Particle> mParticles;
	uint32_t mCount;
	bool mIsBillboard = true;
};
