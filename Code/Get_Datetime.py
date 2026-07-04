from datetime import datetime

def get_date_and_time():
    current = datetime.now()
    current_date = current.strftime("%Y/%m/%d")
    current_time = current.strftime("%H:%M")
    return (current_date, current_time)