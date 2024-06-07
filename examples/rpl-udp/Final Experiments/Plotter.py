import math
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit
from scipy.stats import norm
# Define the filenames
files = ['puttyWith.txt', 'puttyNo.txt', 'Drone.txt','RelayCal.txt','TX-21_puttyWith.txt','TX-21_puttyNo.txt','Drone_TX-21.txt']

# Initialize a dictionary to hold the data for each file
data = {}

# Process each file
for filename in files:
    # Initialize a nested dictionary for the current file
    data[filename] = {}

    # Read the file and process the lines
    with open(filename, 'r') as file:
        # Read the header line and initialize lists for each column
        header = file.readline().strip().split(',')
        for column_name in header:
            data[filename][column_name] = []
        
        # Read the data lines
        for line in file:
            values = line.strip().split(',')
            for column_name, value in zip(header, values):
                # Convert values to appropriate types if necessary
                if value.isdigit():
                    value = int(value)
                elif value.replace('-', '').isdigit():
                    value = int(value)
                else:
                    value = float(value)
                data[filename][column_name].append(value)

# Change count to time and epoch time start time
data["puttyWith.txt"]["Dist"] = []
data["puttyNo.txt"]["Dist"] = []

data["puttyWith.txt"]["time"] = []
data["puttyNo.txt"]["time"] = []

data["puttyWith.txt"]["lost"] = []
data["puttyNo.txt"]["lost"] = []

data["TX-21_puttyWith.txt"]['dist']= []
data["TX-21_puttyNo.txt"]['dist']= []

data["TX-21_puttyWith.txt"]['time']= []
data["TX-21_puttyNo.txt"]['time']= []

data["TX-21_puttyWith.txt"]['lost']= []
data["TX-21_puttyNo.txt"]['lost']= []

for i in range(len(data["puttyWith.txt"]["C"]) ):
    
    data["puttyWith.txt"]["rec"][i] =  data["puttyWith.txt"]["rec"][i] - 232
    data["puttyWith.txt"]["sent"][i] =  data["puttyWith.txt"]["sent"][i] - 232
    data["puttyWith.txt"]["C"][i] =  data["puttyWith.txt"]["C"][i] - 232
    data["puttyWith.txt"]["Dist"].append(math.ceil(data["puttyWith.txt"]["C"][i]/10))
    data["puttyWith.txt"]["time"].append(data["puttyWith.txt"]["C"][i] * 2)
    data["puttyWith.txt"]["lost"].append(data["puttyWith.txt"]["sent"][i] - data["puttyWith.txt"]["rec"][i])
data["puttyWith.txt"]["Dist"][0] = 1

for i in range(len(data["puttyNo.txt"]["C"]) ):
    data["puttyNo.txt"]["Dist"].append(math.ceil(data["puttyNo.txt"]["C"][i]/10))
    data["puttyNo.txt"]["time"].append(data["puttyNo.txt"]["C"][i] * 2)
    data["puttyNo.txt"]["lost"].append(data["puttyNo.txt"]["sent"][i] - data["puttyNo.txt"]["rec"][i])
data["puttyNo.txt"]["Dist"][0] = 1
  
for i in range(len(data["Drone.txt"]["time"])):
    data["Drone.txt"]["time"][i] = data["Drone.txt"]["time"][i] - 1717504971.0 # epoch time of 7'th Drone measurement, as this is where the relay turned on

for i in range(len(data["Drone_TX-21.txt"]["time"])):
    data["Drone_TX-21.txt"]["time"][i] = data["Drone_TX-21.txt"]["time"][i] - 1717602809 # epoch time of 7'th Drone measurement, as this is where the relay turned on

data["RelayCal.txt"]["time"] = []
for i in range(len(data["RelayCal.txt"]["RSSIDB"])):
    data["RelayCal.txt"]["time"].append(i*5)
    data["RelayCal.txt"]["RSSIDC"][i] = - data["RelayCal.txt"]["RSSIDC"][i]
    data["RelayCal.txt"]["RSSIDB"][i] = - data["RelayCal.txt"]["RSSIDB"][i]

for i in range(len(data["TX-21_puttyWith.txt"]["C"]) ):
    data["TX-21_puttyWith.txt"]["rec"][i] =  data["TX-21_puttyWith.txt"]["rec"][i] - 10
    data["TX-21_puttyWith.txt"]["sent"][i] =  data["TX-21_puttyWith.txt"]["sent"][i] - 10
    data["TX-21_puttyWith.txt"]["C"][i] =  data["TX-21_puttyWith.txt"]["C"][i] - 10
    data["TX-21_puttyWith.txt"]["dist"].append(math.ceil(data["TX-21_puttyWith.txt"]["C"][i]/10))
    data["TX-21_puttyWith.txt"]["time"].append(data["TX-21_puttyWith.txt"]["C"][i] * 2)
    data["TX-21_puttyWith.txt"]["lost"].append(data["TX-21_puttyWith.txt"]["sent"][i] - data["TX-21_puttyWith.txt"]["rec"][i])

for i in range(len(data["TX-21_puttyNo.txt"]["C"]) ):
    data["TX-21_puttyNo.txt"]["dist"].append(math.ceil(data["TX-21_puttyNo.txt"]["C"][i]/10))
    data["TX-21_puttyNo.txt"]["time"].append(data["TX-21_puttyNo.txt"]["C"][i] * 2)
    data["TX-21_puttyNo.txt"]["lost"].append(data["TX-21_puttyNo.txt"]["sent"][i] - data["TX-21_puttyNo.txt"]["rec"][i])
data["TX-21_puttyNo.txt"]["dist"][0] = 1



# Add distance to drone 

data['Drone.txt']['dist']= []
data['Drone_TX-21.txt']['dist']= []
curr_drone=0
for j in range(len(data['puttyWith.txt']['C'])):
    if data['Drone.txt']['time'][curr_drone]<data['puttyWith.txt']['time'][j+1]:
        data['Drone.txt']['dist'].append(data['puttyWith.txt']['Dist'][j])
        curr_drone+=1
        if curr_drone>=len(data['Drone.txt']['time']):
            break
curr_drone=0
print(len(data['Drone_TX-21.txt']['time']),len(data['TX-21_puttyWith.txt']['C']))
for j in range(len(data['TX-21_puttyWith.txt']['C'])):

    if data['Drone_TX-21.txt']['time'][curr_drone]<data['TX-21_puttyWith.txt']['time'][j+1]:
        data['Drone_TX-21.txt']['dist'].append(data['TX-21_puttyWith.txt']['dist'][j])
        print(''+str(data['Drone_TX-21.txt']['time'][curr_drone])+','+str((data['TX-21_puttyWith.txt']['time'][j])))
        print(''+str(data['Drone_TX-21.txt']['dist'][curr_drone])+','+str((data['TX-21_puttyWith.txt']['dist'][j]))+'\n')
        curr_drone+=1
        if curr_drone>=len(data['Drone_TX-21.txt']['time']):
            break




    
def print_dictionary():
    for filename in data:
        print(f"Data from {filename}:")
        for column_name in data[filename]:
            print(f"  {column_name}: {data[filename][column_name]}")
        print()

def calibration_plot():
    
    
    DCmean = np.mean(np.array(data["RelayCal.txt"]["RSSIDC"]))
    DCstd = np.std(np.array(data["RelayCal.txt"]["RSSIDC"]))
    DBmean = np.mean(np.array(data["RelayCal.txt"]["RSSIDB"]))
    DBstd = np.std(np.array(data["RelayCal.txt"]["RSSIDB"]))
    
    
    
    plt.figure(figsize=(10, 6))
    ax = plt.axes()
    ax.set_facecolor("gray")
    plt.plot(np.linspace())
    plt.scatter(data["RelayCal.txt"]["time"], data["RelayCal.txt"]["RSSIDC"], color='blue', label='RSSIDC')
    plt.scatter(data["RelayCal.txt"]["time"], data["RelayCal.txt"]["RSSIDB"], color='orange', label='RSSIDB')
    plt.grid(True)
print_dictionary()  






def distance_function_plot():
###### Create data to determine N and power_measurement:#######
    power_measurement = 0
    RSSImean = []
    RSSIstd = []
    dist = []
    currentDist = 0
    amount_n = []
    current_n = 0
    currentRSSIs = []
###### get averages and std for each distance ###########
    for i in range(3,len(data["puttyNo.txt"]["C"])):
    
        if data["puttyNo.txt"]["Dist"][i] != currentDist:
            # add mean and std to lists
            if len(currentRSSIs) != 0:
                RSSImean.append(np.mean(currentRSSIs))
                RSSIstd.append(np.std(currentRSSIs))
            #initialize variables for next distance value
            currentRSSIs = [data["puttyNo.txt"]["RSSI"][i]]
            currentDist = data["puttyNo.txt"]["Dist"][i]
            dist.append(currentDist)
            amount_n.append(current_n)
            current_n = 1
            
        else: # add data to acumulative 
            currentRSSIs.append(data["puttyNo.txt"]["RSSI"][i])
            current_n += 1
    amount_n = amount_n[1:]
    dist = dist[:-1]
    print(RSSImean)

    power_measurement = RSSImean[0]
    def distance_function(RSSI , N):
        return 10 ** ((power_measurement - RSSI) / (10 * N))

    popt, pcov = curve_fit(distance_function, RSSImean, dist, bounds=(2, 4))
    # Optimal value for N
    optimal_N = popt[0]

    print(optimal_N)

    # Theoretical function line:
    theo_RSSI = np.linspace(100, 0, 1000)  # Adjust the range as needed
    # Compute corresponding distances
    theo_dist = distance_function(-theo_RSSI,optimal_N)

    z_value = norm.ppf(0.975)

    lower_bound = np.array(np.abs(RSSImean)) - z_value * np.array((RSSIstd))/np.sqrt(np.array(amount_n))
    upper_bound = np.array(np.abs(RSSImean)) + z_value * np.array((RSSIstd))/np.sqrt(np.array(amount_n))

    ######## dist vs rssi plot no drone#########

    plt.figure(figsize=(10, 6))
    ax = plt.axes()
    ax.set_facecolor("gray")
    plt.scatter(dist, np.abs(RSSImean), color='blue', label='RSSI')
    plt.plot(theo_dist, theo_RSSI, color='red', label=f'Distance function with N = {round(optimal_N,2)} : PM = {power_measurement}')


    plt.fill_between(dist, lower_bound, upper_bound, color='orange', alpha=0.2, label='95% CI')
    plt.xlabel('Distance(m)')
    plt.ylabel('RSSI Mean')
    plt.ylim(40,100)
    plt.yscale("log")
 
    plt.xlim(0,25)
    plt.title('Distance vs. RSSI Mean with Distance function Curve Fit')
    plt.legend()
    plt.grid(True)

#distance_function_plot()
#plt.show()


def relay_vs_no_relay_rssi():

    RSSI_mean_node=[]
    RSSI_mean_root=[]
    RSSI_mean_with=[]
    current_rssi_node=[]
    current_rssi_with=[]
    current_rssi_root=[]
    dist=[]
    dist2=[]
    amount_n=[]
    #time_Drone=data['Drone.txt']['dist']
    #RSSI_Drone_Node=data['Drone.txt']['RSSIDC']
    #RSSI_Drone_Root=data['Drone.txt']['RSSIDB']
    plt.figure(figsize=(10, 6))
    ax = plt.axes()
    current_n=1
    currentDist=1
    ########Calculate means for no drone###########
    for i in range(len(data["puttyWith.txt"]["C"])):
        if data["puttyWith.txt"]["Dist"][i] != currentDist:
            # add mean and std to lists
            if len(current_rssi_with) != 0:
                RSSI_mean_with.append(np.mean(current_rssi_with))
                #RSSIstd.append(np.std(currentRSSIs))
            #initialize variables for next distance value
            current_rssi_with = [data["puttyWith.txt"]["RSSI"][i]]
            currentDist = data["puttyWith.txt"]["Dist"][i]
            dist.append(currentDist)
            amount_n.append(current_n)
            current_n = 1
            
        else: # add data to acumulative 
            current_rssi_with.append(data["puttyWith.txt"]["RSSI"][i])
            current_n += 1

    currentDist=1
    current_n=0
    for i in range(len(data["Drone.txt"]["time"])):
        if data["Drone.txt"]["dist"][i] != currentDist:
            # add mean and std to lists
            if len(current_rssi_node) != 0:
                RSSI_mean_node.append(-np.mean(current_rssi_node))
                RSSI_mean_root.append(-np.mean(current_rssi_root))
                #RSSIstd.append(np.std(currentRSSIs))
            #initialize variables for next distance value
            current_rssi_root = [data["Drone.txt"]["RSSIDB"][i]]
            current_rssi_node = [data["Drone.txt"]["RSSIDC"][i]]
            currentDist = data["Drone.txt"]["dist"][i]
            dist2.append(currentDist)
            amount_n.append(current_n)
            current_n = 1
            
        else: # add data to acumulative 
            current_rssi_node.append(data["Drone.txt"]["RSSIDC"][i])
            current_rssi_root.append(data["Drone.txt"]["RSSIDB"][i])
            current_n += 1




    #plt.scatter(time_Drone, RSSI_Drone_Node, color='blue', label='Drone to Node')
    plt.plot(dist, RSSI_mean_with, color='red', label='root to node')
    plt.plot(dist2, RSSI_mean_root, color='blue', label='relay to root')
    plt.plot(dist2, RSSI_mean_node, color='green', label='relay to node')

    plt.xlabel('Distance(m)')
    plt.ylabel('RSSI')
    plt.ylim(-100,-40)
    ##plt.yscale("log")
 
    plt.xlim(0,25)
    plt.title('RSSI of with/without relay')
    plt.legend()
    plt.grid(True)
    #print(time_)

#relay_vs_no_relay_rssi()
def packages_lost_compare():
    distWith=[]
    distNo=[]
    packetsRecivedWith=[]
    packetsLostWith=[]
    curPacketWith=0
    packetsRecivedNo=[]
    packetsLostNo=[]
    curPacketNo=0
    currentDist=1
    maxLostWith=0
    maxLostNo=0
    plt.subplots(figsize=(10, 6))
    for i in range(len(data["puttyWith.txt"]["C"])):
        if data["puttyWith.txt"]["Dist"][i]>23:
            break
        if data["puttyWith.txt"]["Dist"][i]+1 != currentDist:
            packetsLostWith.append(data['puttyWith.txt']['lost'][i])
            packetsRecivedWith.append(data["puttyWith.txt"]['rec'][i])
            distWith.append(data['puttyWith.txt']['Dist'][i])
            currentDist=data["puttyWith.txt"]["Dist"][i]+1
    currentDist=1
    for i in range(len(data["puttyNo.txt"]["C"])):
        if data["puttyNo.txt"]["Dist"][i]>23:
            break
        if data["puttyNo.txt"]["Dist"][i]+1 != currentDist:
            packetsLostNo.append(data['puttyNo.txt']['lost'][i])
            packetsRecivedNo.append(data["puttyNo.txt"]['rec'][i])
            distNo.append(data['puttyNo.txt']['Dist'][i])
            currentDist=data["puttyWith.txt"]["Dist"][i]+1


    barWidth=0.3
    plt.subplots(figsize=(10, 6))
    br1=np.arange(len(distWith))
    br2=[x+barWidth for x in br1]
    plt.bar(br1, packetsRecivedWith,color='red',width = barWidth, label='Received With Drone',bottom=packetsLostWith)
    plt.bar(br1, packetsLostWith,color='orange',width = barWidth, label='Lost With Drone')
    plt.bar(br2, packetsRecivedNo,color='blue',width = barWidth, label='Received without Drone',bottom=packetsLostNo)
    plt.bar(br2, packetsLostNo,color='green',width = barWidth, label='Lost without drone')
    #plt.plot(dist2, RSSI_mean_root, color='blue', label='relay to root')
    #plt.plot(dist2, RSSI_mean_node, color='green', label='relay to node')
    plt.xticks([r + 1+barWidth for r in range(len(distWith))], 
        distWith)
    plt.legend()
    plt.xlabel('Distance(m)')
    plt.ylabel('packets')
    plt.title("Packet Delivery Rate")

    
def packages_lost_compare_TX21():
    distWith=[]
    distNo=[]
    packetsRecivedWith=[]
    packetsLostWith=[]
    curPacketWith=0
    packetsRecivedNo=[]
    packetsLostNo=[]
    curPacketNo=0
    currentDist=1
    maxLostWith=0
    maxLostNo=0
    plt.subplots(figsize=(10, 6))

    for i in range(len(data["TX-21_puttyWith.txt"]["C"])):
        print(data["TX-21_puttyWith.txt"]["dist"][i])
 #       if data["TX-21_puttyWith.txt"]["dist"][i]>9:
 #           break
        if data["TX-21_puttyWith.txt"]["dist"][i]+1 != currentDist:
            packetsLostWith.append(data['TX-21_puttyWith.txt']['lost'][i])
            packetsRecivedWith.append(data["TX-21_puttyWith.txt"]['rec'][i])
            distWith.append(data['TX-21_puttyWith.txt']['dist'][i])
            currentDist=data["TX-21_puttyWith.txt"]["dist"][i]+1
    currentDist=1
    for i in range(len(data["TX-21_puttyNo.txt"]["C"])):
#        if data["TX-21_puttyNo.txt"]["dist"][i]>9:
#           break
        if data["TX-21_puttyNo.txt"]["dist"][i]+1 != currentDist:
            packetsLostNo.append(data['TX-21_puttyNo.txt']['lost'][i])
            packetsRecivedNo.append(data["TX-21_puttyNo.txt"]['rec'][i])
            distNo.append(data['TX-21_puttyNo.txt']['dist'][i])
            currentDist=data["TX-21_puttyNo.txt"]["dist"][i]+1

    while len(packetsRecivedNo)!=len(packetsRecivedWith):
        packetsRecivedNo.append(0)
        packetsLostNo.append(0)
    barWidth=0.3
    plt.subplots(figsize=(10, 6))
    br1=np.arange(len(distWith))
    br2=[x+barWidth for x in br1]
    plt.bar(br1, packetsRecivedWith,color='red',width = barWidth, label='Received With Drone',bottom=packetsLostWith)
    plt.bar(br1, packetsLostWith,color='orange',width = barWidth, label='Lost With Drone')
    plt.bar(br2, packetsRecivedNo,color='blue',width = barWidth, label='Received without Drone',bottom=packetsLostNo)
    plt.bar(br2, packetsLostNo,color='green',width = barWidth, label='Lost without drone')
    plt.xlabel('Distance(m)')
    plt.ylabel('packets')
    plt.title("Packet Delivery Rate")

    plt.xticks([r +barWidth for r in range(len(distWith))], 
        distWith)

 

    ##plt.yscale("log")
 
    plt.title('Packet loss at low TX power')
    plt.legend()
    plt.grid(True)
#relay_vs_no_relay_rssi()

def position_graph_1step():
    barWidth=0.45
    plt.subplots(figsize=(10, 6))
    br1=np.arange(len(data["Drone_TX-21.txt"]['dist']))
    br2=[x+barWidth for x in br1]
    plt.title('One meter intervals')
    plt.bar(br1,data["Drone_TX-21.txt"]['RSSIBD'],width=barWidth,color='blue',label='drone to root')
    plt.bar(br2,data["Drone_TX-21.txt"]['RSSIDC'],width=barWidth, color='red', label='drone to node')
    plt.xlabel('Measurement')
    plt.ylabel('RSSI')
    plt.legend()
    #plt.xticks([r +barWidth for r in range(len(data["Drone_TX-21.txt"]['RSSIDC']))],range(len(data["Drone_TX-21.txt"]['RSSIDC'])))
def position_graph_middle():
    barWidth=0.45
    plt.subplots(figsize=(10, 6))
    br1=np.arange(len(data["Drone.txt"]['dist']))
    br2=[x+barWidth for x in br1]
    plt.title('Move to middle')
    plt.bar(br1,data["Drone.txt"]['RSSIDB'],width=barWidth,color='blue',label='drone to root')
    plt.bar(br2,data["Drone.txt"]['RSSIDC'],width=barWidth, color='red', label='drone to node')
    #plt.xticks([r +barWidth for r in range(len(data["Drone_TX-21.txt"]['RSSIDC']))],range(len(data["Drone_TX-21.txt"]['RSSIDC'])))
    plt.legend()
    plt.xlabel('Measurement')
    plt.ylabel('RSSI')    



# packages_lost_compare_TX21()
position_graph_1step()
plt.show()
#
#
#
        

#### time vs rssi plot ####
#figtimeVSrssi = plt.figure()
#ax1 = figtimeVSrssi.add_subplot(111)
#
#ax1.set_title('RSSI vs time')
#ax1.set_xlabel('time(s)')
#ax1.set_ylabel('RSSI')
#
#ax1.scatter(data["puttyWith.txt"]["time"][:228],data["puttyWith.txt"]["RSSI"][:228],marker= ".")
#ax1.scatter(data["puttyNo.txt"]["time"][:228],data["puttyNo.txt"]["RSSI"][:228],marker= ".")





# Print the resulting nested dictionary for each file



