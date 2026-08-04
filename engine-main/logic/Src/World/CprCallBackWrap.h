#pragma once
#include "World.h"
#include "cpr/cpr.h"

using namespace BLOCKMAN;

class CprCallBackWrap : public Singleton<CprCallBackWrap>, public ObjectAlloc
{
public:
	CprCallBackWrap(World* pWorld)
		: m_pWorld(pWorld)
	{}

	~CprCallBackWrap() {}

	//// Get methods
	//template <typename... Ts>
	//cpr::Response Get(Ts&&... ts)
	//{
	//	return cpr::Get(std::move(ts)...);
	//}

	//// Get async methods
	//template <typename... Ts>
	//void GetAsync(Ts... ts)
	//{
	//	auto response = cpr::GetAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Get callback methods
	template <typename Then, typename... Ts>
	void GetCallback(Then then, Ts... ts)
	{
		auto callback = cpr::GetCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Post methods
	//template <typename... Ts>
	//cpr::Response Post(Ts&&... ts)
	//{
	//	return cpr::Post(std::move(ts)...);
	//}

	//// Post async methods
	//template <typename... Ts>
	//void PostAsync(Ts... ts) 
	//{
	//	auto response = cpr::PostAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Post callback methods
	template <typename Then, typename... Ts>
	void PostCallback(Then then, Ts... ts)
	{
		auto callback = cpr::PostCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Put methods
	//template <typename... Ts>
	//cpr::Response Put(Ts&&... ts) 
	//{
	//	return cpr::Put(std::move(ts)...);
	//}

	//// Put async methods
	//template <typename... Ts>
	//void PutAsync(Ts... ts) 
	//{
	//	auto response = cpr::PutAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Put callback methods
	template <typename Then, typename... Ts>
	void PutCallback(Then then, Ts... ts)
	{
		auto callback = cpr::PutCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Head methods
	//template <typename... Ts>
	//cpr::Response Head(Ts&&... ts) 
	//{
	//	return cpr::Head(std::move(ts)...);
	//}

	//// Head async methods
	//template <typename... Ts>
	//void HeadAsync(Ts... ts) 
	//{
	//	auto response = cpr::HeadAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Head callback methods
	template <typename Then, typename... Ts>
	void HeadCallback(Then then, Ts... ts)
	{
		auto callback = cpr::HeadCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Delete methods
	//template <typename... Ts>
	//cpr::Response Delete(Ts&&... ts)
	//{
	//	return cpr::Delete(std::move(ts)...);
	//}

	//// Delete async methods
	//template <typename... Ts>
	//void DeleteAsync(Ts... ts) 
	//{
	//	auto response = cpr::DeleteAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Delete callback methods
	template <typename Then, typename... Ts>
	void DeleteCallback(Then then, Ts... ts)
	{
		auto callback = cpr::DeleteCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Options methods
	//template <typename... Ts>
	//cpr::Response Options(Ts&&... ts)
	//{
	//	return cpr::Options(std::move(ts)...);
	//}

	//// Options async methods
	//template <typename... Ts>
	//void OptionsAsync(Ts... ts)
	//{
	//	auto response = cpr::OptionsAsync(std::move(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Options callback methods
	template <typename Then, typename... Ts>
	void OptionsCallback(Then then, Ts... ts)
	{
		auto callback = cpr::OptionsCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

	//// Patch methods
	//template <typename... Ts>
	//cpr::Response Patch(Ts&&... ts)
	//{
	//	return cpr::Patch(std::move(ts)...);
	//}

	//// Patch async methods
	//template <typename... Ts>
	//void PatchAsync(Ts... ts) {
	//	auto response = cpr::PatchAsync(std::(ts)...);
	//	m_pWorld->addCprFutureToList(std::move(response));
	//}

	// Patch callback methods
	template <typename Then, typename... Ts>
	void PatchCallback(Then then, Ts... ts)
	{
		auto callback = cpr::PatchCallback(std::move(then), std::move(ts)...);
		m_pWorld->addCprFutureToList(std::move(callback));
	}

private:
	World * m_pWorld = NULL;
};

