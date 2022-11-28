#!/usr/bin/env groovy

properties(
    [
    buildDiscarder
        (logRotator (
            artifactDaysToKeepStr: '',
            artifactNumToKeepStr: '',
            daysToKeepStr: '14',
            numToKeepStr: ''
        ) ),
    disableConcurrentBuilds(),
    parameters
        ( [
            booleanParam(defaultValue: false, description: 'Adds --no-cache to Docker build command', name: 'noCache'),
            booleanParam(defaultValue: false, description: 'Calls make clean before building the code', name: 'clean')
        ] )
    ]
)

node {


    def SALUSER_HOME = "/home/saluser"
    def BRANCH = (env.CHANGE_BRANCH != null) ? env.CHANGE_BRANCH : env.BRANCH_NAME
    def SAME_CRIO_BRANCH = ["main"]
    def XML_BRANCH = BRANCH in ["main", "tickets/DM-37100"] ? BRANCH : "develop"

    stage('Cloning sources')
    {
        dir("ts_cRIOcpp") {
            git branch: (BRANCH in SAME_CRIO_BRANCH) ? BRANCH : "develop", url: 'https://github.com/lsst-ts/ts_cRIOcpp'
        }
        dir("ts_vms") {
            checkout scm
        }
    }

    stage('Building dev container')
    {
        VMSsim = docker.build("lsstts/vms_sim:" + env.BRANCH_NAME.replace("/", "_"), "--target crio-develop --build-arg XML_BRANCH=$XML_BRANCH " + (params.noCache ? "--no-cache " : " ") + "$WORKSPACE/ts_vms")
    }

    stage("Running tests")
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]) {
             VMSsim.inside("--entrypoint=''") {
                 if (params.clean) {
                 sh """
                    cd $WORKSPACE/ts_cRIOcpp
                    make clean
                    cd $WORKSPACE/ts_vms
                    make clean
                 """
                 }
                 sh """
                    source $SALUSER_HOME/.setup_salobj.sh
    
                    export PATH=\$CONDA_PREFIX/bin:$PATH
                    cd $WORKSPACE/ts_cRIOcpp
                    make
    
                    cd $WORKSPACE/ts_vms
                    LIBS_FLAGS="-L\$CONDA_PREFIX/lib" make simulator
                 """
             }
        }
    }

    stage('Build documentation')
    {
         VMSsim.inside("--entrypoint=''") {
             sh """
                source $SALUSER_HOME/.setup_salobj.sh
                cd $WORKSPACE/ts_vms
                make doc
             """
         }
    }

    stage('Running container')
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]){
            VMSsim.inside("--entrypoint=''") {
                sh """
                    source $SALUSER_HOME/.setup_salobj.sh

                    export LSST_DDS_PARTITION_PREFIX=test
    
                    cd $WORKSPACE/ts_vms
                    ./ts-VMSd -c SettingFiles M1M3 &
    
                    echo "Waiting for 30 seconds"
                    sleep 30
    
                    cd $SALUSER_HOME/repos
                    ./ts_sal/test/MTVMS/cpp/src/sacpp_MTVMS_start_commander Default
                    sleep 30
                    killall ts-VMSd
                """
            }
        }
    }

    if (BRANCH == "master" || BRANCH == "develop")
    {
        stage('Publish documentation')
        {
            withCredentials([usernamePassword(credentialsId: 'lsst-io', usernameVariable: 'LTD_USERNAME', passwordVariable: 'LTD_PASSWORD')]) {
                M1M3sim.inside("--entrypoint=''") {
                    sh """
                        source $SALUSER_HOME/.setup_salobj.sh
                        ltd upload --product ts-vms --git-ref """ + BRANCH + """ --dir $WORKSPACE/ts_vms/doc/html
                    """
                }
            }
        }
    }
}
