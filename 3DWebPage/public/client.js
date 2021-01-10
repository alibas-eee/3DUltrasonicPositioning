import * as THREE from '/build/three.module.js';
import {OrbitControls} from '/jsm/controls/OrbitControls.js';
import Stats from '/jsm/libs/stats.module.js';

var cData = {x:0,y:0,z:0};
var cData1 = {x:0,y:0,z:0};
var c = 0;

var chart_data = {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'X',
            backgroundColor: "lightsalmon",
            borderColor: "lightsalmon",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
            fill: false,
        }, {
            label: 'Y',
            fill: false,
            backgroundColor: "lightblue",
            borderColor: "lightblue",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
        }, {
            label: 'Z',
            fill: false,
            backgroundColor: "aquamarine",
            borderColor: "aquamarine",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
        }]
    },
    options: {
        maintainAspectRatio: false,
        title: {
            display: true,
            text: 'Position Values'
        }
    }
};


var chart_data1 = {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: '0',
            backgroundColor: "khaki",
            borderColor: "khaki",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
            fill: false,
        }, {
            label: '1',
            fill: false,
            backgroundColor: "lightblue",
            borderColor: "lightblue",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
        }, {
            label: '2',
            fill: false,
            backgroundColor: "coral)",
            borderColor: "coral",
            borderWidth : 1,
            pointRadius: 1,
            data: [],
        }]
    },
    options: {
        maintainAspectRatio: false, title: {
            display: true,
            text: 'Sensor Values'
        }
    }
};


var ctx = document.getElementById("Chart0").getContext('2d');

var chart = new Chart(ctx, chart_data);


var ctx1 = document.getElementById("Chart1").getContext('2d');

var chart1 = new Chart(ctx1, chart_data1);


function addData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets[0].data.push(data.x);
    chart.data.datasets[1].data.push(data.y);
    chart.data.datasets[2].data.push(data.z);
    chart.update(0);
}

function removeData(chart) {
    if(chart.data.labels.length > 100)
    {
        chart.data.labels.shift();
        chart.data.datasets.forEach((dataset) => {
            dataset.data.shift();
        });
        chart.update(0);
    }
}

var count = 0;
var dist = {x:0,y:0,z:10, s0:0, s1:0, s2:0, v:0};
 // Create WebSocket connection.
 const socket = new WebSocket('ws://localhost:3030');

 // Connection opened
 socket.addEventListener('open', function (event) {
     console.log('Connected to WS Server')
 });

 // Listen for messages
 socket.addEventListener('message', function (event) {
     console.log('Message from server ', event.data);

    dist =  JSON.parse(event.data);
    cData.x = dist.x;
    cData.y = dist.y; 
    cData.z = dist.z;

    cData1.x = dist.s0;
    cData1.y = dist.s1; 
    cData1.z = dist.s2;
    c++;
    addData(chart, c, cData)
    addData(chart1, c, cData1)
    removeData(chart);
    removeData(chart1);
 });

 const sendMessage = () => {
     count++;
    socket.send(count);
 }



const scene = new THREE.Scene();

const camera = new THREE.PerspectiveCamera(100, window.innerWidth / window.innerHeight, 1, 1000);

camera.position.set(-25,0,25); // Set position 
camera.lookAt(new THREE.Vector3(0,0,0)); // Set look at coordinate 

const light = new THREE.PointLight( 0xff0000, 1, 100 );
light.position.set( 50, 50, 50 );
scene.add( light );


const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
//document.body.appendChild(renderer.domElement);
document.getElementById("threeDiv").appendChild(renderer.domElement);


const controls = new OrbitControls(camera, renderer.domElement);

var material_line = new THREE.LineBasicMaterial({
    color: 0x0000ff
});


const geoSphere = new THREE.SphereGeometry( 0.5, 32, 32 );
const material = new THREE.MeshBasicMaterial( {color: 0xaa00aa} );
const sphere = new THREE.Mesh( geoSphere, material );

sphere.position.z = 20; 

scene.add( sphere );


const cgeo = new THREE.CylinderGeometry( 1, 1, 0.5, 32 );
const cmaterial = new THREE.MeshBasicMaterial( {color: 0xe0e000} );
const cmaterial1 = new THREE.MeshBasicMaterial( {color: 0x00e0e0} );
const cMesh0 = new THREE.Mesh( cgeo, cmaterial );
const cMesh1 = new THREE.Mesh( cgeo, cmaterial );
const cMesh2 = new THREE.Mesh( cgeo, cmaterial );

const cMesh3 = new THREE.Mesh( cgeo, cmaterial1 );

cMesh0.position.x = -10;
cMesh1.position.x = 10;
cMesh2.position.y = -13.8;
cMesh3.position.y = -4.6;

cMesh0.rotation.x = Math.PI / 2;;
cMesh1.rotation.x = Math.PI / 2;;
cMesh2.rotation.x = Math.PI / 2;;
cMesh3.rotation.x = Math.PI / 2;;



const pgeometry = new THREE.PlaneGeometry( 200, 200, 32 );
const pmaterial = new THREE.MeshBasicMaterial( {color: 0x222222, wireframe:true} );
const plane0 = new THREE.Mesh( pgeometry, pmaterial );
const plane1 = new THREE.Mesh( pgeometry, pmaterial );
const plane2 = new THREE.Mesh( pgeometry, pmaterial );
plane1.rotation.x = Math.PI / 2;;
plane1.position.y = -100;
plane1.position.z = 100;

plane2.rotation.y = Math.PI / 2;;
plane2.position.x = 100;
plane2.position.z = 100;


scene.add( plane0 );
scene.add( plane1 );
scene.add( plane2 );


//Create line between two mesh
function createLine( m0,  m1, clr)
{
    var _material = new THREE.LineDashedMaterial( {
        color: 0xffffff,
        linewidth: 1,
        scale: 5,
        dashSize: 5,
        gapSize: 5,
    } );

    //var material = new THREE.MeshBasicMaterial( {color: clr} );
    var geoL = new THREE.Geometry();
    geoL.vertices.push( m0.position, m1.position);
    return new THREE.Line( geoL, _material );
}

/*
var pA ={ position : {x : -50, y : -50, z : 0}};
var pB ={ position : {x : 50, y : -50, z : 0}};
var pC ={ position : {x : 50, y : 50, z : 0}};
var pD ={ position : {x : 50, y : -50, z : 100}};

var lX = createLine(pA, pB, 0xaaaaaa);
var lY = createLine(pB, pC, 0xaaaaaa);
var lZ = createLine(pB, pD, 0xaaaaaa );
scene.add( lX );
scene.add( lY );
scene.add( lZ );*/

var line0 = createLine(cMesh0, sphere, 0xaaaaaa);
var line1 = createLine(cMesh1, sphere, 0xaaaaaa);
var line2 = createLine(cMesh2, sphere, 0xaaaaaa );

scene.add( line0 );
scene.add( line1 );
scene.add( line2 );

scene.add( cMesh0 );
scene.add( cMesh1 );
scene.add( cMesh2 );
scene.add( cMesh3 );

window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
    render();
}, false);

const stats = Stats();
document.body.appendChild(stats.dom);

var animate = function () {
    requestAnimationFrame(animate);
    if(dist.v == 1)
    {
        sphere.position.x = dist.x;
        sphere.position.y = dist.y;
        sphere.position.z = dist.z;
    }

console.log(dist.z);
    line0.geometry.verticesNeedUpdate = true;
    line1.geometry.verticesNeedUpdate = true;
    line2.geometry.verticesNeedUpdate = true;

    controls.update();
    render();
    stats.update();
    sendMessage();
};

function render() {
    renderer.render(scene, camera);
}

animate();