#!/usr/bin/python

import requests,re,sys
from gi.repository import Notify

def print_usage():
    print("You didn't specify any arguments, therefore manticore has been set to default.\nUsage : ./marc_watcher.py username [optional : name]")

def get_data(url):
    req = requests.get(url, allow_redirects=False)
    data = req.text
    
    try:
        data = data.split("<main class")[1].split("</main")[0]
        return data
    except IndexError:
        print("Error while fetching "+username+"'s informations :")
        print("Couldn't find given string '<main class' in received resource")
        return None

def parse_data(data):
    data = data.replace("&nbsp;", " ")
    searchObj = re.search(r'Score.*?(\d+)',data)
    return searchObj.group(1)

def check_diff(score):
    filename = "{0}_scores.txt".format(name.lower())
    try:
        fichier = open(filename, "r")
        stored_score = fichier.read()
        fichier.close()
        if stored_score != score:
            text = "{0} has gained points !\n He went from {1} to {2} !".format(name, stored_score, score)
            fichier = open(filename, "w")
            fichier.write(score)
        else:
            text = "{0}'s score hasn't changed ({1}). What a lamer....".format(name, score)

    except FileNotFoundError:
        fichier = open(filename, "w")
        fichier.write(score)
        fichier.close()
        text = "No database found, updating..."
        
    return text

if __name__ == "__main__":

    if not len(sys.argv[1:]):
        print_usage()
        username = "manticore"
        name = "Marc"
    else:
        username = sys.argv[1:][0] 
        if len(sys.argv[1:]) == 1:
            name = "Marc"
        else:
            name = sys.argv[1:][1]

    url = "http://www.root-me.org/{0}".format(username)

    data = get_data(url)
    score = parse_data(data)
    notification_content = check_diff(score)

    Notify.init ("{0} Watcher".format(name))
    notification = Notify.Notification.new("{0} Watcher".format(name), notification_content, "dialog-information")
    notification.show()
