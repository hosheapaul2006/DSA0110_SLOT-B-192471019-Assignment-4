"""Render coursework UML as ordinary editable SVG vectors, without a report."""
from pathlib import Path
from html import escape
ROOT = Path(__file__).resolve().parents[1]

def diagram(task, width, height, definitions, relations):
    nodes = {}
    for name, x, y, w, attrs, methods, abstract in definitions:
        nodes[name] = (x, y, w, 82 + max(1, len(attrs))*23 + max(1, len(methods))*23, attrs, methods, abstract)
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
           '<rect width="100%" height="100%" fill="white"/>',
           '<defs><marker id="inherit" markerWidth="18" markerHeight="16" refX="17" refY="8" orient="auto"><path d="M1,1 L17,8 L1,15 Z" fill="white" stroke="#26384a" stroke-width="1.5"/></marker><marker id="owns" markerWidth="20" markerHeight="12" refX="1" refY="6" orient="auto"><path d="M1,6 L9,1 L17,6 L9,11 Z" fill="#26384a"/></marker></defs>']
    def point(name, side):
        x,y,w,h,*_ = nodes[name]
        return {'top':(x+w/2,y),'bottom':(x+w/2,y+h),'left':(x,y+h/2),'right':(x+w,y+h/2)}[side]
    for child, parent, mode, label, sides, bends in relations:
        a=point(child,sides[0]);b=point(parent,sides[1]);pts=[a]+bends+[b]
        marker='marker-end="url(#inherit)"' if mode=='inherit' else 'marker-start="url(#owns)"'
        out.append('<polyline points="'+' '.join(f'{x},{y}' for x,y in pts)+'" fill="none" stroke="#26384a" stroke-width="2" '+marker+'/>')
        if label:
            x=(pts[0][0]+pts[1][0])/2;y=(pts[0][1]+pts[1][1])/2
            anchor = 'middle' if pts[0][1] == pts[1][1] else 'start'
            x += 0 if anchor == 'middle' else 8
            out.append(f'<text x="{x}" y="{y-8}" text-anchor="{anchor}" font-family="Arial" font-size="17" fill="#374151">{escape(label)}</text>')
    for name,(x,y,w,h,attrs,methods,abstract) in nodes.items():
        out.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="2" fill="#f5f8fb" stroke="#26384a" stroke-width="2"/>')
        title=('<<abstract>> ' if abstract else '')+name
        out.append(f'<text x="{x+w/2}" y="{y+29}" text-anchor="middle" font-family="Arial" font-weight="bold" font-size="20">{escape(title)}</text>')
        out.append(f'<path d="M{x},{y+44} H{x+w}" stroke="#26384a"/>')
        row=y+68
        for value in attrs or ['(no additional fields)']:
            out.append(f'<text x="{x+12}" y="{row}" font-family="Arial" font-size="17">{escape(value)}</text>');row+=23
        out.append(f'<path d="M{x},{row-8} H{x+w}" stroke="#26384a"/>')
        row+=15
        for value in methods or ['(constructors omitted)']:
            out.append(f'<text x="{x+12}" y="{row}" font-family="Arial" font-size="17">{escape(value)}</text>');row+=23
    out.append('</svg>')
    (ROOT/task/'diagrams/hierarchy.svg').write_text('\n'.join(out)+'\n',encoding='utf-8',newline='\n')

diagram('Task01',1200,850,[
 ('Staff',400,20,380,['- name_: string','- id_: int'],['+ pay(): double = 0','+ virtual ~Staff()'],True),
 ('SalariedStaff',130,310,420,['- basic_: double','- daRate_: double'],['# ordinaryPay(): double'],True),
 ('GuestFaculty',800,310,340,['- hours_: double','- hourlyRate_: double'],['+ pay(): double override'],False),
 ('Professor',50,640,330,['- research_: double'],['+ pay(): double override'],False),
 ('LabAssistant',460,640,340,[],['+ pay(): double override'],False),
],[('SalariedStaff','Staff','inherit','',('top','bottom'),[(340,260),(590,260)]),('GuestFaculty','Staff','inherit','',('top','bottom'),[(970,260),(590,260)]),('Professor','SalariedStaff','inherit','',('top','bottom'),[(215,580),(340,580)]),('LabAssistant','SalariedStaff','inherit','',('top','bottom'),[(630,580),(340,580)])])

diagram('Task02',1250,900,[
 ('Person',480,20,350,['- name_: string; - id_: int','- contact_: Contact'],['+ name(), id(), contact()','+ virtual ~Person()'],False),
 ('Contact',950,45,270,['- phone_: string'],['+ phone()'],False),
 ('Student',350,340,340,['- programme_: Programme'],['+ programme()'],False),
 ('Programme',20,345,280,['+ name: string','+ cgpa: double'],[],False),
 ('Staff',860,340,340,['- department_: string','- salary_: double'],['+ salary(), department()'],False),
 ('ResearchScholar',80,700,370,['- fellowship_: double'],['+ fellowship()'],False),
 ('TeachingAssistant',650,700,380,[],['+ constructor initializes Person'],False),
],[('Student','Person','inherit','public virtual',('top','bottom'),[(520,270),(655,270)]),('Staff','Person','inherit','public virtual',('top','bottom'),[(1030,270),(655,270)]),('Person','Contact','owns','has-a',('right','left'),[]),('Student','Programme','owns','',('left','right'),[]),('ResearchScholar','Student','inherit','',('top','bottom'),[(265,620),(520,620)]),('TeachingAssistant','Student','inherit','public',('top','bottom'),[(840,610),(520,610)]),('TeachingAssistant','Staff','inherit','public',('top','bottom'),[(840,610),(1030,610)])])

diagram('Task06',1600,1570,[
 ('Vehicle',540,20,440,['- registration_: string','- dailyRate_: double','- rented_: bool; - bookedDays_: int'],['+ rent(days), returnRental(days, units)','+ charge(days, units): Bill = 0','+ kind(): text = 0; + virtual ~Vehicle()'],True),
 ('Bicycle',30,400,310,[],['+ charge(): daily only','+ kind()'],False),
 ('MotorVehicle',600,400,350,['- service_: ServiceDate'],['# details()'],True),
 ('ServiceDate',1080,400,390,['- year_, month_, day_: int'],['+ calendar validation; + text()'],False),
 ('FuelVehicle',210,650,380,['- tank_: EnergyStore (litres)','- pricePerLitre_: double'],['# fuelCost(litres)','+ fuelLevel(); # consume(litres)'],True),
 ('ElectricVehicle',1040,650,430,['- battery_: EnergyStore (kWh)','- pricePerKWh_: double'],['# electricityCost(kWh)','+ capacity/level(); # consume(kWh)'],True),
 ('EnergyStore',630,720,340,['- capacity_, level_: double'],['+ validate(units), consume(units)','+ capacity(), level()'],False),
 ('TwoWheeler',20,1020,300,[],['+ charge(); + kind()'],False),
 ('Car',370,1020,290,['- seating_: Seating'],['+ charge(); + kind()'],False),
 ('ElectricCar',930,1020,300,['- seating_: Seating'],['+ charge(); + kind()'],False),
 ('ElectricBus',1260,1020,310,['- seating_: Seating'],['+ charge(); + kind()','(new extension file only)'],False),
 ('Seating',700,1300,330,['- seats_: int'],['+ seats()'],False),
 ('Fleet',40,1280,550,['- vehicles_[8]: Vehicle* (borrowed views)','- owners_[8]: unique_ptr<Vehicle>','- count_: size_t'],['+ add(owner); + find(registration)','+ rent(), returnRental(), list()'],False),
],[('Bicycle','Vehicle','inherit','',('top','bottom'),[(185,350),(760,350)]),('MotorVehicle','Vehicle','inherit','',('top','bottom'),[]),('MotorVehicle','ServiceDate','owns','service date',('right','left'),[]),('FuelVehicle','MotorVehicle','inherit','',('top','bottom'),[(400,590),(775,590)]),('ElectricVehicle','MotorVehicle','inherit','',('top','bottom'),[(1255,590),(775,590)]),('FuelVehicle','EnergyStore','owns','',('right','left'),[]),('ElectricVehicle','EnergyStore','owns','',('left','right'),[]),('TwoWheeler','FuelVehicle','inherit','',('top','bottom'),[(170,940),(400,940)]),('Car','FuelVehicle','inherit','',('top','bottom'),[(515,940),(400,940)]),('ElectricCar','ElectricVehicle','inherit','',('top','bottom'),[(1080,940),(1255,940)]),('ElectricBus','ElectricVehicle','inherit','extension',('top','bottom'),[(1415,940),(1255,940)]),('Car','Seating','owns','',('bottom','top'),[(515,1240),(865,1240)]),('ElectricCar','Seating','owns','',('bottom','top'),[(1080,1240),(865,1240)]),('ElectricBus','Seating','owns','',('bottom','top'),[(1415,1260),(865,1260)]),('Fleet','Vehicle','owns','',('left','left'),[(15,1380),(15,130)])])
print('Created three standalone UML SVG diagrams')
