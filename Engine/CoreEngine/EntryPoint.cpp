#include "common_pch.h"
#include "AppDefines.h"
#include "Core/Graphics/Mesh/JMesh.h"
#include "Core/Utils/FileIO/JSerialization.h"


/*class CharacterAttribute : public AutoSerializer
{
public:
	void SetTestData()
	{
		this->hp.Set(99);
		this->job.Set(7);
		this->speed.Set(150.0f);
		this->test.Set(new int32_t(100));

		this->isAlive = true;
	}

	void SetHp(int64_t _hp)
	{
		this->hp.Set(_hp);
	}

	void SetSpeed(float _speed)
	{
		this->speed.Set(_speed);
	}

	void SetJob(int32_t _job)
	{
		this->job.Set(_job);
	}

	void SetAlive(bool _isAlive)
	{
		this->isAlive = _isAlive;
	}

	void Print()
	{
		std::cout << "hp:" << hp.Get() << std::endl;
		std::cout << "job:" << job.Get() << std::endl;
		std::cout << "speed:" << speed.Get() << std::endl;

		std::cout << "has:" << *test.Get() << std::endl;


	}

private:
	// serializable.
	JProperty<int64_t>  hp{this};
	JProperty<int32_t>  job{this};
	JProperty<float>    speed{this};
	JProperty<int32_t*> test{this};

	// non-serializable.
	bool isAlive{};
};*/



int main(int argc, char** argv)
{

	// std::cout << bIsSerializable<Sample> << std::endl;
	// std::cout << bIsSerializable<Application> << std::endl;
	// std::ifstream outfile("Game/has.json");
	// std::ofstream infile("Game/has.json", std::ios::binary);
	//
	// //------------------------------------------------
	// std::cout << "----- serailize all -----" << std::endl;
	// CharacterAttribute foo{};
	// foo.SetTestData();
	// // foo -> byte array.
	// std::vector<char> bufferFoo(foo.GetSerializationBufferSize(), 0);
	// foo.Serialize(infile);
	// foo.Print();
	//
	// // byte array -> bar.
	// std::cout << std::endl;
	// std::cout << "----- deseralize all -----" << std::endl;
	// CharacterAttribute bar{};
	// bar.DeSerialize(outfile);
	// bar.Print();
	//
	RUN(Name_Engine_Version, HD_X, HD_Y)

	
	
	return 0;
}
