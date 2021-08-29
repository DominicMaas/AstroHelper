import React, { useEffect } from "react";
import {
  AppBar,
  Typography,
  Toolbar,
  FormControl,
  InputLabel,
  Select,
  MenuItem,
  Container,
  makeStyles,
  createStyles,
  Theme,
  FormHelperText,
  TextField,
  Button,
  Backdrop,
  CircularProgress,
  Dialog,
  IconButton,
} from "@material-ui/core";
import { Alert } from "@material-ui/lab";
import CloseIcon from "@material-ui/icons/Close";

const useStyles = makeStyles((theme: Theme) =>
  createStyles({
    formControl: {
      width: "100%",
      marginBottom: theme.spacing(3),
    },
    backdrop: {
      zIndex: theme.zIndex.drawer + 1,
      color: "#fff",
    },
    appBar: {
      display: "relative",
    },
    title: {
      marginLeft: theme.spacing(2),
      flex: 1,
    },
    img: {
      display: "Block",
      objectFit: "contain",
      width: "100%",
    },
  })
);

class KVPair {
  constructor(value: string, choices: string[]) {
    this.value = value;
    this.choices = choices;
  }

  public value: string;
  public choices: string[];
}

let loopCount: number = 0;

function NightPage() {
  const classes = useStyles();

  const [loading, setLoading] = React.useState<boolean>(true);
  const [error, setError] = React.useState<string>();

  const [recShutter, setRecShutter] = React.useState<string>("?");

  const [iso, setIso] = React.useState<KVPair>(new KVPair("", [""]));
  const [shutterSpeed, setShutterSpeed] = React.useState(new KVPair("", [""]));
  const [fStop, setFStop] = React.useState(new KVPair("", [""]));

  const [delay, setDelay] = React.useState("5");
  const [takes, setTakes] = React.useState("10");

  const [running, setRunning] = React.useState(false);

  const [stopText, setStopText] = React.useState("Stop");

  const [dialogOpen, setDialogOpen] = React.useState(false);
  const [previewData, setPreviewData] = React.useState("");

  useEffect(() => {
    if (loading) return; // Prevent initial value being set

    setLoading(true);

    fetch("http://raspberrypi:8080/set-config-item/iso", {
      method: "POST",
      body: iso.value,
    })
      .then((res) => {
        setLoading(false);
      })
      .catch((res) => {
        setError(res);
        setLoading(false);
      });
  }, [iso]);

  useEffect(() => {
    if (loading) return; // Prevent initial value being set

    setLoading(true);

    fetch("http://raspberrypi:8080/set-config-item/shutterspeed", {
      method: "POST",
      body: shutterSpeed.value,
    })
      .then((res) => {
        setLoading(false);
      })
      .catch((res) => {
        setError(res);
        setLoading(false);
      });
  }, [shutterSpeed]);

  useEffect(() => {
    if (loading) return; // Prevent initial value being set

    setLoading(true);

    fetch("http://raspberrypi:8080/set-config-item/f-number", {
      method: "POST",
      body: fStop.value,
    })
      .then((res) => {
        setLoading(false);
      })
      .catch((res) => {
        setError(res);
        setLoading(false);
      });
  }, [fStop]);

  // Set the defaults (this only runs when the component is first mounted)
  useEffect(() => {
    refresh();
  }, []);

  const refresh = () => {
    setLoading(true);

    let a = fetch("http://raspberrypi:8080/get-config-item/iso")
      .then((res) => res.json())
      .then((res) => {
        let iso = new KVPair(res.value, res.choices);
        setIso(iso);
      })
      .catch((res) => {
        setError("The camera is not connected, or the device is not on!");
      });

    let b = fetch("http://raspberrypi:8080/get-config-item/shutterspeed")
      .then((res) => res.json())
      .then((res) => {
        let shutterSpeed = new KVPair(res.value, res.choices);
        setShutterSpeed(shutterSpeed);
      })
      .catch((res) => {
        setError("The camera is not connected, or the device is not on!");
      });

    let c = fetch("http://raspberrypi:8080/get-config-item/f-number")
      .then((res) => res.json())
      .then((res) => {
        let fStop = new KVPair(res.value, res.choices);
        setFStop(fStop);
      })
      .catch((res) => {
        setError("The camera is not connected, or the device is not on!");
      });

    let d = fetch("http://raspberrypi:8080/get-config-item/focallength")
      .then((res) => res.json())
      .then((res) => {
        let focal = parseFloat(res.value);
        let exposeTime = 500 / focal;
        setRecShutter(Math.round(exposeTime) + "");
      })
      .catch(() => {
        setRecShutter("???");
      });

    // Set loading to false once complete
    Promise.all([a, b, c, d]).then(() => {
      setLoading(false);
    });
  };

  const captureImage = async () => {
    if (running) {
      loopCount = Number.MAX_VALUE; // This forces the counter to stop
      setRunning(false);
      return;
    }

    setRunning(true);

    loopCount = 0;
    const loopFunc = async () => {
      // At the end
      if (loopCount >= Number.parseInt(takes)) {
        setRunning(false);
        return;
      }

      setStopText(
        "Stop (" + (loopCount + 1) + "/" + Number.parseInt(takes) + ")"
      );

      // Run the call
      try {
        await fetch("http://raspberrypi:8080/capture-image", {
          method: "POST",
        });
        setLoading(false);
      } catch (e) {
        setError(e);
        setLoading(false);
      }

      loopCount++;

      setTimeout(loopFunc, 1000 * Number.parseFloat(delay));
    };

    // Call straight away
    await loopFunc();
  };

  const capturePreview = () => {
    // This may take some time
    setLoading(true);

    fetch("http://raspberrypi:8080/capture-preview")
      .then((res) => res.blob())
      .then((res) => {
        setLoading(false);
        setDialogOpen(true);
        setPreviewData(URL.createObjectURL(res));
      })
      .catch((res) => {
        setError(res);
        setLoading(false);
      });
  };

  const handleClose = () => {
    setDialogOpen(false);
  };

  // The UI
  return (
    <>
      <Backdrop className={classes.backdrop} open={loading}>
        <CircularProgress color="inherit" />
      </Backdrop>
      <Dialog fullScreen open={dialogOpen} onClose={handleClose}>
        <AppBar className={classes.appBar}>
          <Toolbar>
            <IconButton
              edge="start"
              color="inherit"
              onClick={handleClose}
              aria-label="close"
            >
              <CloseIcon />
            </IconButton>
            <Typography variant="h6" className={classes.title}>
              Image Preview
            </Typography>
          </Toolbar>
        </AppBar>
        <br /> <br /> <br />
        <br /> <br /> <br />
        <img src={previewData} className={classes.img} alt="Preview Image" />
      </Dialog>

      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6">Night Photography</Typography>
        </Toolbar>
      </AppBar>

      <br />

      <Container>
        {error ? (
          <>
            <Alert severity="error">{error}</Alert>
            <br />
          </>
        ) : (
          ""
        )}

        <FormControl className={classes.formControl} variant="standard">
          <InputLabel id="select-iso-label">ISO</InputLabel>
          <Select
            labelId="select-iso-label"
            id="select-iso"
            value={iso?.value}
            onChange={(v) =>
              setIso(new KVPair(v.target.value as string, iso.choices))
            }
          >
            {iso?.choices.map((v) => (
              <MenuItem key={v} value={v}>
                {v}
              </MenuItem>
            ))}
          </Select>
        </FormControl>

        <FormControl className={classes.formControl} variant="standard">
          <InputLabel id="select-shutter-speed-label">Shutter Speed</InputLabel>
          <Select
            labelId="select-shutter-speed-label"
            id="select-shutter-speed"
            value={shutterSpeed.value}
            onChange={(v) =>
              setShutterSpeed(
                new KVPair(v.target.value as string, shutterSpeed.choices)
              )
            }
          >
            {shutterSpeed?.choices.map((v) => (
              <MenuItem key={v} value={v}>
                {v}
              </MenuItem>
            ))}
          </Select>

          <FormHelperText>
            Recommended shutter speed of max {recShutter}s (500 Rule)
          </FormHelperText>
        </FormControl>

        <FormControl className={classes.formControl} variant="standard">
          <InputLabel id="select-f-stop-label">FStop</InputLabel>
          <Select
            labelId="select-f-stop-label"
            id="select-f-stop"
            value={fStop.value}
            onChange={(v) =>
              setFStop(new KVPair(v.target.value as string, fStop.choices))
            }
          >
            {" "}
            {fStop?.choices.map((v) => (
              <MenuItem key={v} value={v}>
                {v}
              </MenuItem>
            ))}
          </Select>
        </FormControl>

        <FormControl className={classes.formControl} variant="standard">
          <TextField
            label="Delay (seconds)"
            id="select-delay"
            value={delay}
            onChange={(v) => setDelay(v.target.value as string)}
          />
          <FormHelperText>The delay in-between each photo</FormHelperText>
        </FormControl>

        <FormControl className={classes.formControl}>
          <TextField
            label="Takes"
            id="select-takes"
            value={takes}
            onChange={(v) => setTakes(v.target.value as string)}
          />
          <FormHelperText>How many photos to take</FormHelperText>
        </FormControl>

        <FormControl className={classes.formControl}>
          <Button variant="contained" color="primary" onClick={captureImage}>
            {running ? stopText : "Start"}
          </Button>
        </FormControl>

        <FormControl className={classes.formControl}>
          <Button variant="outlined" color="primary" onClick={capturePreview}>
            Capture Preview
          </Button>
        </FormControl>

        <FormControl className={classes.formControl}>
          <Button variant="outlined" onClick={refresh}>
            Refresh
          </Button>
          <FormHelperText>
            Refresh after adjusting the camera focus or zoom
          </FormHelperText>
        </FormControl>
      </Container>

      <br />
      <br />
    </>
  );
}

export default NightPage;
