#include "EditorOptions.h"

#include <fstream>

#include "Debug.h"

int cs::editor::EditorOptions::windowWidth{ 800 };
int cs::editor::EditorOptions::windowHeight{ 600 };
int cs::editor::EditorOptions::windowXPos{ 100 };
int cs::editor::EditorOptions::windowYPos{ 100 };
Vector3 cs::editor::EditorOptions::cameraPosition{ Vector3(0.0f) };
Vector3 cs::editor::EditorOptions::cameraRotation{ Vector3(0.0f) };

const std::string cs::editor::EditorOptions::optionsFilepath{ "options.ini" };

void cs::editor::EditorOptions::LoadSettings()
{
	std::string _settingsString{ "" };
	std::ifstream _file{ optionsFilepath };
	SettingsMode _settingsMode{ SettingsMode::NONE };

	if (!_file || _file.bad() || _file.peek() == std::ifstream::traits_type::eof())
	{
		Debug::LogWarning("Something went wrong when opening the editor settings file.");

		if (!_file)
			Debug::LogIssue("File does not exist.");
		if (_file.bad())
			Debug::LogIssue("File is corrupted.");
		if (_file.peek() == std::ifstream::traits_type::eof())
			Debug::LogIssue("File is empty.");

		_file.close();

		std::ofstream _newFile{ optionsFilepath };
		_newFile << GetDefaultSettings();
		_newFile.close();

		_file.open(optionsFilepath);
	}

	while (std::getline(_file, _settingsString))
	{
		if (_settingsString.empty())
			continue;

		if (_settingsString == "[window]")
		{
			_settingsMode = SettingsMode::WINDOW_SIZE;
			continue;
		}
		else if (_settingsString == "[editor_camera]")
		{
			_settingsMode = SettingsMode::EDITOR_CAMERA;
			continue;
		}

		switch (_settingsMode)
		{
		case SettingsMode::NONE:
			break;
		case SettingsMode::WINDOW_SIZE:
		{
			size_t _nextPos{ _settingsString.find_first_of(',') };

			windowWidth = std::stoi(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			windowHeight = std::stoi(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			windowXPos = std::stoi(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			windowYPos = std::stoi(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);

			break;
		}
		case SettingsMode::EDITOR_CAMERA:
		{
			size_t _nextPos{ _settingsString.find_first_of(',') };

			// POSITION

			cameraPosition.x = std::stof(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			cameraPosition.y = std::stof(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			cameraPosition.z = std::stof(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			// ROTATION

			cameraRotation.x = std::stof(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			cameraRotation.y = std::stof(_settingsString.substr(0, _nextPos));
			_settingsString = _settingsString.substr(_nextPos + 1);
			_nextPos = _settingsString.find_first_of(',');

			cameraRotation.z = std::stof(_settingsString.substr(0, _nextPos));

			break;
		}
		}

		_settingsMode = SettingsMode::NONE;
	}

	_file.close();
}

void cs::editor::EditorOptions::SaveSettings()
{
	std::ofstream _file{ optionsFilepath };

	// Hardcoded trash... (but it is what I have time for)
	_file << "[window]\n";
	_file << windowWidth << ',' << windowHeight << ',' << windowXPos << ',' << windowYPos << '\n';
	_file << "[editor_camera]\n";
	_file << WriteVector3(cameraPosition) << ',' << WriteVector3(cameraRotation) << '\n';
}

std::string cs::editor::EditorOptions::WriteVector3(const Vector3& vector)
{
	return std::to_string(vector.x) + ',' + std::to_string(vector.y) + ',' + std::to_string(vector.z);
}

std::string cs::editor::EditorOptions::GetDefaultSettings()
{
	return "[window]\n800,600,100,100\n[editor_camera]\n10,10,10,0,0,0\n";
}
