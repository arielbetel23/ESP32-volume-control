import requests

weather_API_file_path = r"C:\Users\ariel\OneDrive\שולחן העבודה - Copy\שולחן העבודה\Electronics Projects\Weather_API_Key.txt"

def get_API_Key(file_path):
    with open(file_path, 'r') as file:
        return file.read().strip()

def get_weather():
    api_key = get_API_Key(weather_API_file_path)
    url = "https://api.openweathermap.org/data/2.5/weather"
    params = {
        "q": "Even Yehuda,IL",
        "appid": api_key,
        "units": "metric"
    }
    response = requests.get(url, params=params)
    data = response.json()
    return data

def extract_weather():
    data = get_weather()
    temperature = round(data["main"]["temp"])
    description = data["weather"][0]["description"]
    return temperature, description